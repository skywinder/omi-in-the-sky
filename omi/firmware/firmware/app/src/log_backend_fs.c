#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/fs/fs.h>
#include <string.h>

LOG_MODULE_REGISTER(log_backend_fs);

#define LOG_FILE_PREFIX "log"
#define LOG_FILE_EXT ".txt"
#define LOG_FILE_PATH CONFIG_LOG_BACKEND_FS_DIR "/"
#define MAX_LOG_FILES 10
#define MAX_LOG_SIZE CONFIG_LOG_BACKEND_FS_FILE_SIZE

static struct fs_file_t log_file;
static bool file_ready = false;
static K_MUTEX_DEFINE(log_mutex);
static uint8_t current_file_num = 0;
static char current_file_path[64];
static uint32_t current_file_size = 0;

// Forward declaration
static int create_new_log_file(void);

/**
 * @brief Initialize the filesystem log backend
 *
 * Creates a log file on the SD card in the specified directory
 *
 * @return 0 on success, negative error code on failure
 */
int log_backend_fs_init(void)
{
    printk("Initializing filesystem log backend\n");

    // Check if logs directory exists
    struct fs_dirent entry;
    int ret = fs_stat(CONFIG_LOG_BACKEND_FS_DIR, &entry);

    if (ret != 0) {
        printk("Logs directory not found, attempting to create it\n");

        ret = fs_mkdir(CONFIG_LOG_BACKEND_FS_DIR);
        if (ret != 0 && ret != -EEXIST) {
            printk("Failed to create logs directory: %d\n", ret);
            return ret;
        }
    }

    // Create the initial log file
    ret = create_new_log_file();
    if (ret != 0) {
        printk("Failed to create initial log file: %d\n", ret);
        return ret;
    }

    printk("Filesystem log backend initialized successfully\n");
    return 0;
}

static int create_new_log_file(void)
{
    int ret;
    char file_name[32];

    // Close the previous file if it was open
    if (file_ready) {
        fs_close(&log_file);
        file_ready = false;
    }

    // Generate new file name with timestamp
    snprintf(file_name, sizeof(file_name), "%s.%04d%s", LOG_FILE_PREFIX, current_file_num, LOG_FILE_EXT);
    snprintf(current_file_path, sizeof(current_file_path), "%s%s", LOG_FILE_PATH, file_name);

    // Create new file
    fs_file_t_init(&log_file);
    ret = fs_open(&log_file, current_file_path, FS_O_CREATE | FS_O_WRITE);
    if (ret < 0) {
        printk("Failed to create log file: %d\n", ret);
        return ret;
    }

    // Write header with timestamp
    char header[64];
    int64_t uptime = k_uptime_get();
    snprintf(header, sizeof(header),
             "\n=== Log started at %lld ms (uptime) ===\n\n",
             uptime);

    fs_write(&log_file, header, strlen(header));
    fs_sync(&log_file);

    current_file_size = strlen(header);
    current_file_num = (current_file_num + 1) % MAX_LOG_FILES;
    file_ready = true;

    return 0;
}

/**
 * Simple log message processing - writes timestamp and rotates files when needed
 */
static void process(const struct log_backend *const backend,
                  struct log_msg *msg)
{
    if (!file_ready) {
        return;
    }

    char buffer[128];
    int length;

    k_mutex_lock(&log_mutex, K_FOREVER);

    // Get current timestamp
    int64_t uptime = k_uptime_get();

    // Format a simple log message with timestamp
    length = snprintf(buffer, sizeof(buffer), "[%lld ms] Log message\n", uptime);

    if (length > 0) {
        // Check if we need to create a new file
        if (current_file_size + length > MAX_LOG_SIZE) {
            if (create_new_log_file() != 0) {
                k_mutex_unlock(&log_mutex);
                return;
            }
        }

        fs_write(&log_file, buffer, length);
        current_file_size += length;
        fs_sync(&log_file);
    }

    k_mutex_unlock(&log_mutex);
}

static void panic(struct log_backend const *const backend)
{
    if (file_ready) {
        const char *msg = "\n!!! SYSTEM PANIC !!!\n\n";
        fs_write(&log_file, msg, strlen(msg));
        fs_sync(&log_file);
    }
}

static void dropped(const struct log_backend *const backend, uint32_t cnt)
{
    if (file_ready) {
        char buffer[64];
        int length = snprintf(buffer, sizeof(buffer),
                             "\n*** WARNING: Dropped %u log messages ***\n", cnt);
        fs_write(&log_file, buffer, length);
        fs_sync(&log_file);
    }
}

// Define the backend API structure with the required callbacks
static const struct log_backend_api log_backend_fs_api = {
    .process = process,
    .panic = panic,
    .dropped = dropped,
};

// Register the backend with auto-activation enabled
LOG_BACKEND_DEFINE(log_backend_fs, log_backend_fs_api, true);