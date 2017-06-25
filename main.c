#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "keys.h"
#include "gui.h"
#include "process.h"
#include "cmd_line.h"

/************************* PREDEFINITIONS *************************/
/// Menu
void menu0(), menu1(), menu2();
void menu0_add_key(), menu0_read_keys(), menu0_save_keys(), menu0_choose_active(), menu0_remove();
void remove_key_option(), remove_all_keys_option();
void show_encr_submenu(), show_decr_submenu();
void one_file_encr(), more_files_encr(), regex_encr();
void one_file_decr(), more_files_decr(), regex_decr(), all_keys_decr();
static char* get_one_string_input(char*, char*, int);

/***************************** STATIC *****************************/
static List *key_list = NULL;
static Key *active_key = NULL;

static Menu main_menu[] =
{
    {"Keys", menu0, "Keys options"},
    {"Encryption/Decryption", menu1, "Choose mode and file"},
    {"Exit", menu2, "Exit"},
    {"", (Func)NULL, ""}
};

static Menu keys_menu[] =
{
    {"Add key", menu0_add_key, "Add new key to the list"},
    {"Read from file", menu0_read_keys, "Read keys from the file"},
    {"Save to file", menu0_save_keys, "Save keys from list to the file"},
    {"Choose active key", menu0_choose_active, "Choose key from the list to be used"},
    {"Remove key(s)", menu0_remove, "Remove key/keys from the list"},
    {"", (Func)NULL, ""}
};

static Menu remove_keys_submenu[] =
{
    {"Remove key", remove_key_option, "Select key from the list to remove"},
    {"Remove all keys", remove_all_keys_option, "Remove all keys from the list"},
    {"", (Func)NULL, ""}
};

static Menu encrypt_menu[] =
{
    {"Encryption", show_encr_submenu, "Choose mode and file to encrypt"},
    {"Decryption", show_decr_submenu, "Choose mode and file to decrypt"},
    {"", (Func)NULL, ""}
};

static Menu encr_submenu[] =
{
    {"One file", one_file_encr, "Chose one file to encrypt"},
    {"More files", more_files_encr, "Choose more files to encrypt"},
    {"Regex", regex_encr, "Insert regex pattern for file names to encrypt"},
    {"", (Func)NULL, ""}
};

static Menu decr_submenu[] =
{
    {"One file", one_file_decr, "Chose one file to decrypt"},
    {"More files", more_files_decr, "Choose more files to decrypt"},
    {"Regex", regex_decr, "Insert regex pattern for file names to decrypt"},
    {"All keys", all_keys_decr, "Find the matching key for decryption"},
    {"", (Func)NULL, ""}
};

void menu0() {
    do_menu(keys_menu);
}

void menu1() {
    do_menu(encrypt_menu);
}

void menu2() {
    finish();
}

void menu0_add_key() {
    char *field_descriptions[7];
    char *buf[6];
    char error_msg[MAX_STR_LEN];
    int sizes[6] = {6, 4, 16, 33, 8, 8};
    Key *key = (Key*) malloc(sizeof(Key));

    buf[0] = key->type;
    buf[1] = key->mode;
    buf[2] = key->key_name;
    buf[3] = key->key[0];
    buf[4] = key->key[1];
    buf[5] = key->key[2];

    field_descriptions[0] = "Type:";
    field_descriptions[1] = "Mode:";
    field_descriptions[2] = "Key name:";
    field_descriptions[3] = "Key 1:";
    field_descriptions[4] = "Key 2:";
    field_descriptions[5] = "Key 3:";
    field_descriptions[6] = 0;

    buf[0][0] = buf[1][0] = buf[2][0] = buf[3][0] = buf[4][0] = buf[5][0] = '\0';
    /// Probaj da uspe ovaj default message!
    //strcpy(buf[4], "Only for 3DES");
    //strcpy(buf[5], "Only for 3DES");

    if (get_input(field_descriptions, buf, sizes) == KEY_ESC)
        error_message("No key inputed!", 1);
    else if (check_correct_key(key, error_msg))
        error_message(error_msg, 1);
    else {
        char msg[MAX_KEY_ROW_LEN] = "Added key: ";
        error_message(strcat(msg, key->key_name), 0);

        add_key(key_list, key);
        //if (!active_key)
        //    active_key = key;
    }
}

void menu0_read_keys() {
    if (read_keys(KEYS_FILE, &key_list))
        error_message("Unable to open file with keys! Keys not read...", 1);
    else if (!key_list->head)
        error_message("File with keys is empty!", 1);

    active_key = NULL;
}

void menu0_save_keys() {
    if (write_keys(key_list, KEYS_FILE))
        error_message("Unable to open file with keys! Keys not written...", 1);
}

void menu0_choose_active() {
    Key *key = select_key(key_list);
    if (key) {
        char msg[MAX_KEY_ROW_LEN] = "Selected active key: ";
        active_key = key;
        error_message(strcat(msg, key->key_name), 0);
    }
}

void menu0_remove() {
    do_menu(remove_keys_submenu);
}

void remove_key_option() {
    Key *key = select_key(key_list);

    if (key) {
        char msg[MAX_KEY_ROW_LEN] = "Removed key: ";
        int flag = key == active_key;

        error_message(strcat(msg, key->key_name), 0);
        remove_key(key_list, key);

        if (flag) /// MOZDA DA GA STAVIM NA NULL SVAKAKO
            //active_key = key_list->head ? (Key*)key_list->head->info : NULL;
            active_key = NULL;
    }
}

void remove_all_keys_option() {
    remove_keys(key_list);
    active_key = NULL;
    error_message("All keys removed!", 0);
}

void show_encr_submenu() {
    do_menu(encr_submenu);
}

void show_decr_submenu() {
    do_menu(decr_submenu);
}

void one_file_encr() {
    char file_path[MAX_STR_LEN] = {0};
    char error_msg[MAX_STR_LEN];

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (get_filepath(file_path) == KEY_ESC)
        error_message("File path not inputed!", 1);
    else if (encrypt_file(file_path, active_key, error_msg))
        error_message(error_msg, 1);
    else
        error_message("File encrypted!", 0);
}

void more_files_encr() {
    char file_path[MAX_STR_LEN] = {0};
    FILE *log_file = fopen("log.txt", "w");

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (get_filepath(file_path) == KEY_ESC)
        error_message("File path not inputed!", 1);
    else if (encrypt_more_files(file_path, active_key, log_file))
        error_message("Unable to open file!", 1);
    else
        error_message("See log.txt for info about encryption.", 0);

    if (log_file)
        fclose(log_file);
}

void regex_encr() {
    char regex_pattern[MAX_STR_LEN] = {0};
    char error_msg[MAX_STR_LEN];
    FILE *log_file = fopen("log.txt", "w");

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (!get_one_string_input("Regex pattern:", regex_pattern, 60))
        error_message("Regex pattern not inputed!", 1);
    else if (encrypt_regex_files(regex_pattern, active_key, error_msg, log_file))
        error_message(error_msg, 1);
    else
        error_message("See log.txt for info about encryption.", 0);

    if (log_file)
        fclose(log_file);
}

void one_file_decr() {
    char file_path[MAX_STR_LEN] = {0};
    char error_msg[MAX_STR_LEN];

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (get_filepath(file_path) == KEY_ESC)
        error_message("File path not inputed!", 1);
    else if (decrypt_file(file_path, active_key, error_msg))
        error_message(error_msg, 1);
    else
        error_message("File decrypted!", 0);
}

void more_files_decr() {
    char file_path[MAX_STR_LEN] = {0};
    FILE *log_file = fopen("log.txt", "w");

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (get_filepath(file_path) == KEY_ESC)
        error_message("File path not inputed!", 1);
    else if (decrypt_more_files(file_path, active_key, log_file))
        error_message("Unable to open file!", 1);
    else
        error_message("See log.txt for info about decryption.", 0);

    if (log_file)
        fclose(log_file);
}

void regex_decr() {
    char regex_pattern[MAX_STR_LEN] = {0};
    char error_msg[MAX_STR_LEN];
    FILE *log_file = fopen("log.txt", "w");

    if (!active_key)
        error_message("No active key selected!", 1);
    else if (!get_one_string_input("Regex pattern:", regex_pattern, 60))
        error_message("Regex pattern not inputed!", 1);
    else if (decrypt_regex_files(regex_pattern, active_key, error_msg, log_file))
        error_message(error_msg, 1);
    else
        error_message("See log.txt for info about decryption.", 0);

    if (log_file)
        fclose(log_file);
}

void all_keys_decr() {
    char file_path[MAX_STR_LEN] = {0};
    char error_msg[MAX_STR_LEN];

    if (!key_list->head)
        error_message("No keys in the list!", 1);
    else if (get_filepath(file_path) == KEY_ESC)
        error_message("File path not inputed!", 1);
    else {
        ListElement *curr;
        Key *key;

        for (curr = key_list->head; curr; curr = curr->next)
            if (!decrypt_file(file_path, (Key*)curr->info, error_msg)) {
                sprintf(error_msg, "Matching key found: %s", ((Key*)curr->info)->key_name);
                error_message(error_msg, 0);
                break;
            }
        if (!curr)
            error_message("No matching key found!", 1);
    }
}

static char* get_one_string_input(char *description, char *buf, int field_size) {
    char *field_name[2];
    char *field_buf[1];
    int f_size[1] = {field_size};

    field_name[0] = description;
    field_name[1] = 0;
    field_buf[0] = buf;

    return (get_input(field_name, field_buf, f_size) == KEY_ESC) ? NULL : buf;
}

int main(int argc, char *argv[]) {
    /// initialization
    key_list = init_list();
    read_keys(KEYS_FILE, &key_list);
    active_key = NULL;

    /// MAIN WORK
    if (argc == 1) /// gui
        start_menu(main_menu, "Encryption/Decryption SREDI OVAJ TITLE!");
    else /// cmd line
        process_command(argc, argv, key_list);

    /// free
    remove_list(key_list);
    free(key_list);
    key_list = active_key = NULL;
    return 0;
}
