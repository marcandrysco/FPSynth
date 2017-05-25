#include <sys/inotify.h>
#include "../common.h"


/**
 * Notify structure.
 *   @fd: The file descriptor.
 *   @watch: The watch list.
 *   @change: The change information.
 */
struct sys_notify_t {
	sys_fd_t fd;
	struct sys_watch_t *watch;

	struct sys_change_t change;
};

/**
 * Watch structure.
 *   @id: The inotify descriptor.
 *   @dir, name: The directoy and name components.
 *   @next: The next watch.
 */
struct sys_watch_t {
	int id;
	char *dir, *name;

	struct sys_watch_t *next;
};


/**
 * Create a change notifier.
 *   &returns: The notifier.
 */
struct sys_notify_t *sys_notify_new(void)
{
	struct sys_notify_t *notify;

	notify = malloc(sizeof(struct sys_notify_t));
	notify->fd = inotify_init();
	notify->watch = NULL;

	return notify;
}

/**
 * Delete a change notifier.
 *   @notify: The notifier.
 */
void sys_notify_delete(struct sys_notify_t *notify)
{
	struct sys_watch_t *watch;

	while(notify->watch != NULL) {
		watch = notify->watch;
		notify->watch = watch->next;

		erase(watch->name);
		free(watch->dir);
		free(watch);
	}

	close(notify->fd);
	free(notify);
}


/**
 * Add a path to the change notifier.
 *   @notify: The notifier.
 *   @path: The path.
 *   @id: The identifier.
 *   &returns: Error.
 */
char *sys_notify_add(struct sys_notify_t *notify, const char *path, int *id)
{
#define onexit erase(name); free(dir);
	int ifd;
	char *dir, *name;
	struct sys_watch_t **watch;

	if(fs_isdir(path)) {
		dir = strdup(path);
		name = NULL;
	}
	else {
		dir = malloc(fs_dirname_len(path) + 1);
		fs_dirname_str(dir, path);

		name = malloc(fs_basename_len(path) + 1);
		fs_basename_str(name, path);
	}

	if(!fs_exists(dir))
		fail("Unknown path '%s'.", path);

	ifd = inotify_add_watch(notify->fd, dir, IN_MODIFY | IN_MOVED_TO | IN_ATTRIB);
	if(id != NULL)
		*id = ifd;

	watch = &notify->watch;
	while(*watch != NULL)
		watch = &(*watch)->next;

	*watch = malloc(sizeof(struct sys_watch_t));
	(*watch)->id = ifd;
	(*watch)->next = NULL;
	(*watch)->dir = dir;
	(*watch)->name = name;

	return NULL;
#undef onexit
}


/**
 * Retrieve the descriptor for the change notifier.
 *   @notify: The notifier.
 *   &returns: The descriptor.
 */
sys_fd_t sys_notify_fd(struct sys_notify_t *notify)
{
	return notify->fd;
}

/**
 * Retrieve the basename from the path. The returned pointer points into the
 * string passed in as 'path'; the returned value is valid so long as 'path'
 * is not modified.
 *   @path: The path.
 *   &returns; The basename.
 */
const char *hax_basename(const char *path)
{
	const char *ptr = strrchr(path, '/');

	return ptr ? (ptr + 1) : path;
}

/**
 * Process a poll of the notifier.
 *   @notify: The notifier.
 *   @poll: The poll array.
 *   &returns: The change structure if processed.
 */
struct sys_change_t *sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll)
{
	ssize_t rd;
	struct sys_watch_t *watch;
	struct inotify_event *event;
	uint8_t buf[sizeof(struct inotify_event) + NAME_MAX + 1];

	if(poll->revents == 0)
		return NULL;

	rd = read(notify->fd, &buf, sizeof(buf));
	if(rd < 0)
		fatal("Read failed. %s.", strerror(errno));


	event = (void *)buf;
	if(event->len == 0)
		return NULL;

	for(watch = notify->watch; watch != NULL; watch = watch->next) {
		if(watch->id == event->wd)
			break;
	}

	if(watch == NULL)
		return NULL;

	if(watch->name != NULL) {
		if(strcmp(event->name, watch->name) != 0)
			return NULL;
	}

	snprintf(notify->change.name, 256, "%s", event->name);
	//strcpy(notify->name, event->name);
	//if(strcmp(hax_basename(notify->path), event->name) == 0)

	return &notify->change;
}
