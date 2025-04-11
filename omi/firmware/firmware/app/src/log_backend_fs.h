#ifndef LOG_BACKEND_FS_H
#define LOG_BACKEND_FS_H

/**
 * @brief Initialize the filesystem log backend
 *
 * This function should be called after the SD card is mounted.
 * It creates the logs directory if it doesn't exist and initializes
 * the first log file.
 *
 * @return 0 on success, negative error code on failure
 */
int log_backend_fs_init(void);

#endif /* LOG_BACKEND_FS_H */