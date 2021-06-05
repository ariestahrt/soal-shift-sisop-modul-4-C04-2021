#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 1024
#define ARR_SIZE 100

#define ROT13 1
#define VIGENERE 2

time_t my_time;
struct tm * timeinfo;

static const char *dirpath = "/home/ariestaheart/Downloads";
static const char *fs_dir = "/home/ariestaheart/Modul4/Demo";
static const char *logpath = "/home/ariestaheart/SinSeiFS.log";

char log_msg[SIZE*2];
char current_readdir[SIZE];

// === ATOZ

char *atoz_directory[ARR_SIZE];
int atoz_last_idx = 0;

void atoz_insert(char* dir){
    atoz_directory[atoz_last_idx] = malloc(strlen(dir) + 1);
    strcpy(atoz_directory[atoz_last_idx], dir);
    atoz_last_idx++;
    printf("AtoZ INSERTED :: %s\n", atoz_directory[atoz_last_idx-1]);
}

bool atoz_contains(char *dir){
    for(int i=0; i<atoz_last_idx; i++){
        if(!strcmp(atoz_directory[i], dir)) return true;
    }
    return false;
}

int atoz_insubstr(char *dir){
    for(int i=0; i<atoz_last_idx; i++){
        if(strstr(dir, atoz_directory[i])){
            return i;
        }
    }
    return -1;
}

void atoz_remove(char *dir){
    for(int i=0; i<atoz_last_idx; i++){
        if(!strcmp(atoz_directory[i], dir)){
            strcpy(atoz_directory[i], "xxx..............xxx");
        }
    }
}

// === END OF ATOZ

// === RX

struct rx {
    char DIR[SIZE];    
    int CHIPER;    
};

struct rx rx_directory[ARR_SIZE];
int rx_last_idx = 0;

void rx_insert(char* dir, int chiper){
    sprintf(rx_directory[rx_last_idx].DIR, "%s", dir);
    rx_directory[rx_last_idx].CHIPER = chiper;
    rx_last_idx++;
}

bool rx_contains(char *dir){
    for(int i=0; i<rx_last_idx; i++){
        if(!strcmp(rx_directory[i].DIR, dir)) return true;
    }
    return false;
}

int rx_insubstr(char *dir){
    int to_return = -1;
    for(int i=0; i<rx_last_idx; i++){
        if(strstr(dir, rx_directory[i].DIR)){
            to_return = i;
        }
    }
    return to_return;
}

void rx_remove(char *dir){
    for(int i=0; i<rx_last_idx; i++){
        if(!strcmp(rx_directory[i].DIR, dir)){
            strcpy(rx_directory[i].DIR, "xxx..............xxx");
        }
    }
}

// === END OF RX

char *strrev(char *str){
	  char *p1, *p2;

	  if (! str || ! *str)
			return str;
	  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	  {
			*p1 ^= *p2;
			*p2 ^= *p1;
			*p1 ^= *p2;
	  }
	  return str;
}

char* convertToCharPtr(char *str){
	int len=strlen(str);
	char* ret = malloc((len+1) * sizeof(char));
	for(int i=0; i<len; i++){
		ret[i] = str[i];
	}
	ret[len] = '\0';
	return ret;
}

char* getTimeNow(){
	time (&my_time);
	timeinfo = localtime (&my_time);

	char day[10], month[10], year[10], hour[10], minute[10], second[10];

	sprintf(day, "%d", timeinfo->tm_mday);
	if(timeinfo->tm_mday < 10) sprintf(day, "0%d", timeinfo->tm_mday);

	sprintf(month, "%d", timeinfo->tm_mon+1);
	if(timeinfo->tm_mon+1 < 10) sprintf(month, "0%d", timeinfo->tm_mon+1);

	sprintf(year, "%d", timeinfo->tm_year+1900);

	sprintf(hour, "%d", timeinfo->tm_hour);
	if(timeinfo->tm_hour < 10) sprintf(hour, "0%d", timeinfo->tm_hour);

	sprintf(minute, "%d", timeinfo->tm_min);
	if(timeinfo->tm_min < 10) sprintf(minute, "0%d", timeinfo->tm_min);

	sprintf(second, "%d", timeinfo->tm_sec);
	if(timeinfo->tm_sec < 10) sprintf(second, "0%d", timeinfo->tm_sec);

	char datetime_now[100];
	sprintf(datetime_now, "%s-%s-%s_%s:%s:%s", year, month, day, hour, minute, second);
	char* ret=convertToCharPtr(datetime_now);
	return ret;
}

void removeSubstr(char *string, char *sub){
    while(strstr(string, sub)){
        char *match;
        int len = strlen(sub);
        if ((match = strstr(string, sub))) {
            *match = '\0';
            strcat(string, match+len);
        }
    }
}

void logs(){
	time (&my_time);
	timeinfo = localtime(&my_time);

	char *timeNow = getTimeNow();
	printf("%s : %s\n", timeNow, log_msg);
}

bool isAtoz(char *str){
    if(strlen(str) >= 6){
        if(str[0] == '/' && str[1] == 'A' && str[2] == 't' && str[3] == 'o' && str[4] == 'Z' && str[5] == '_'){
            return true;
        }
    }
    return false;
}

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

char *aes_crypt(char *str){
    for(int i=0; i<strlen(str); i++){
        if(str[i] >= 'A' && str[i] <= 'Z'){
            str[i] = 'Z' + 'A' - str[i];
        }else if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] = 'z' + 'a' - str[i];
        }
    }

    return str;
}

char *chaesarEncrypt(char* str, int shift){
    int len = strlen(str);
    char *ret = malloc((len+1) * sizeof(char));

    for(int i=0; i<len; i++){
        if(str[i] >= 97 && str[i] <= 122){
            int asciinum = str[i] - 'a';
            asciinum = 97 + (asciinum+shift)%26;
            ret[i] = asciinum;
        }else if (str[i] >= 65 && str[i] <= 90){
            int asciinum = str[i] - 'A';
            asciinum = 65 + (asciinum+shift)%26;
            ret[i] = asciinum;
        }else{
            ret[i] = str[i];
        }
    }
    ret[len] = '\0';

    return ret;
}

char *vignereEncrypt(char *str, char *key) {    
    char *str_copy = malloc((strlen(str)+1) * sizeof(char));
    sprintf(str_copy, "%s", str);
    char temp[SIZE]; sprintf(temp, "%s", str);

    int i = 0, curKey = 0;
    for(i = 0; i < strlen(str_copy); i++) {
        if(str_copy[i] >= 'a' && str_copy[i] <= 'z') {
            str_copy[i] = str_copy[i] - 'a' + 'A';
        }
    }

    for(int i = 0; i < strlen(str_copy); i++) {
        if(curKey == strlen(key)) curKey = 0;

        if(str_copy[i] >= 'A' && str_copy[i] <= 'Z')
            str_copy[i] = ((str_copy[i] + key[curKey]) % 26);
            
        if(temp[i] >= 'a' && temp[i] <= 'z')
            str_copy[i] += 'a';
        else if(temp[i] >= 'A' && temp[i] <= 'Z')
            str_copy[i] += 'A';
        else
            curKey--;
        
        curKey++;
    }

    str_copy[strlen(str)] = 0;
    return str_copy;
}

char* vignereDecrypt(char *str, char *key) {    
    char *str_copy = malloc((strlen(str)+1) * sizeof(char));
    sprintf(str_copy, "%s", str);
    char temp[SIZE]; sprintf(temp, "%s", str);

    int i = 0, curKey = 0;
    for(i = 0; i < strlen(str_copy); i++) {
        if(str_copy[i] >= 'a' && str_copy[i] <= 'z') {
            str_copy[i] = str_copy[i] - 'a' + 'A';
        }
    }

    for(int i = 0; i < strlen(str_copy); i++) {
        if(curKey == strlen(key)) curKey = 0;

        if(str_copy[i] >= 'A' && str_copy[i] <= 'Z') {
            str_copy[i] = str_copy[i] - key[curKey];

            if(str_copy[i] < 0)
                str_copy[i] += 26;
        }

        if(temp[i] >= 'a' && temp[i] <= 'z')
            str_copy[i] += 'a';
        else if(temp[i] >= 'A' && temp[i] <= 'Z')
            str_copy[i] += 'A';
        else
            curKey--;
        
        curKey++;
    }

    str_copy[strlen(str)] = 0;
    return str_copy;
}

void encryptRecursively(char *path, int CHIPER){
    sprintf(log_msg, "[~] encryptRecursively path : %s", path); logs();
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char from_rename[SIZE]; sprintf(from_rename, "%s/%s", path, entry->d_name);
            
            char *encrypted;
            if(CHIPER == ROT13){
                encrypted = chaesarEncrypt(entry->d_name, 13);
            }else if(CHIPER == VIGENERE){
                encrypted = vignereEncrypt(entry->d_name, "SISOP");
            }

            char to_rename[SIZE]; sprintf(to_rename, "%s/%s", path, encrypted);

            rename(from_rename, to_rename);
            sprintf(log_msg, "[~] encrypt, rename : %s to %s", from_rename, to_rename); logs();

            char next_path[SIZE];
            sprintf(next_path, "%s/%s", path, encrypted);
            encryptRecursively(next_path, CHIPER);
        } else {
            // This is file
            char from_rename[SIZE]; sprintf(from_rename, "%s/%s", path, entry->d_name);
            char to_rename[SIZE];
            char *encrypted;

            char *dir_ext = get_filename_ext(entry->d_name);
            if(strlen(dir_ext) > 0){
                char to_decrypt[SIZE]; sprintf(to_decrypt, "%s", entry->d_name);
                to_decrypt[strlen(to_decrypt) - strlen(dir_ext) -1] = 0;

                if(CHIPER == ROT13){
                    encrypted = chaesarEncrypt(to_decrypt, 13);
                }else if(CHIPER == VIGENERE){
                    encrypted = vignereEncrypt(to_decrypt, "SISOP");
                }
                sprintf(to_rename, "%s/%s.%s", path, encrypted, dir_ext);
            }else{
                if(CHIPER == ROT13){
                    encrypted = chaesarEncrypt(entry->d_name, 13);
                }else if(CHIPER == VIGENERE){
                    encrypted = vignereEncrypt(entry->d_name, "SISOP");
                }
                sprintf(to_rename, "%s/%s", path, encrypted);
            }

            rename(from_rename, to_rename);
            sprintf(log_msg, "[~] encrypt, rename : %s to %s", from_rename, to_rename); logs();
        }
    }
    closedir(dir);
}


void decryptRecursively(char *path, int CHIPER){
    sprintf(log_msg, "[~] decryptRecursively path : %s", path); logs();
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char from_rename[SIZE]; sprintf(from_rename, "%s/%s", path, entry->d_name);
            
            char *decrypted;
            if(CHIPER == ROT13){
                decrypted = chaesarEncrypt(entry->d_name, 13);
            }else if(CHIPER == VIGENERE){
                decrypted = vignereDecrypt(entry->d_name, "SISOP");
            }

            char to_rename[SIZE]; sprintf(to_rename, "%s/%s", path, decrypted);

            rename(from_rename, to_rename);
            sprintf(log_msg, "[~] encrypt, rename : %s to %s", from_rename, to_rename); logs();

            char next_path[SIZE];
            sprintf(next_path, "%s/%s", path, decrypted);
            encryptRecursively(next_path, CHIPER);
        } else {
            // This is file
            char from_rename[SIZE]; sprintf(from_rename, "%s/%s", path, entry->d_name);
            char to_rename[SIZE];
            char *decrypted;

            char *dir_ext = get_filename_ext(entry->d_name);
            if(strlen(dir_ext) > 0){
                char to_decrypt[SIZE]; sprintf(to_decrypt, "%s", entry->d_name);
                to_decrypt[strlen(to_decrypt) - strlen(dir_ext) -1] = 0;

                if(CHIPER == ROT13){
                    decrypted = chaesarEncrypt(to_decrypt, 13);
                }else if(CHIPER == VIGENERE){
                    decrypted = vignereDecrypt(to_decrypt, "SISOP");
                }
                sprintf(to_rename, "%s/%s.%s", path, decrypted, dir_ext);
            }else{
                if(CHIPER == ROT13){
                    decrypted = chaesarEncrypt(entry->d_name, 13);
                }else if(CHIPER == VIGENERE){
                    decrypted = vignereEncrypt(entry->d_name, "SISOP");
                }
                sprintf(to_rename, "%s/%s", path, decrypted);
            }

            rename(from_rename, to_rename);
            sprintf(log_msg, "[~] encrypt, rename : %s to %s", from_rename, to_rename); logs();
        }
    }
    closedir(dir);
}

char *getStrBetween(char *str, char *PATTERN1, char *PATTERN2){
    char *target = NULL;
    char *start, *end;

    if ( start = strstr( str, PATTERN1 ) ){
        start += strlen( PATTERN1 );
        if ( end = strstr( start, PATTERN2 ) ){
            target = ( char * )malloc( end - start + 1 );
            memcpy( target, start, end - start );
            target[end - start] = '\0';
        }
    }
    if(target == NULL){
        return "";
    }else{
        return target;
    }
}

void put_logs(char *message){
    char log_file[SIZE];
    sprintf(log_file, "%s/logs.txt", dirpath);

    printf("PUTS LOGS CALLED----\n");
    printf("LOGS FILE :: %s\n", log_file);
    
    FILE* fptr_logs = fopen(log_file, "a");
    fprintf(fptr_logs, "%s\r\n", message);
    fclose(fptr_logs);
}

void put_systemlogs(char* level, char *command, char *desc){
    time (&my_time);
	timeinfo = localtime (&my_time);

	char day[10], month[10], year[10], hour[10], minute[10], second[10];

	sprintf(day, "%d", timeinfo->tm_mday);
	if(timeinfo->tm_mday < 10) sprintf(day, "0%d", timeinfo->tm_mday);

	sprintf(month, "%d", timeinfo->tm_mon+1);
	if(timeinfo->tm_mon+1 < 10) sprintf(month, "0%d", timeinfo->tm_mon+1);

	sprintf(year, "%d", timeinfo->tm_year+1900);

	sprintf(hour, "%d", timeinfo->tm_hour);
	if(timeinfo->tm_hour < 10) sprintf(hour, "0%d", timeinfo->tm_hour);

	sprintf(minute, "%d", timeinfo->tm_min);
	if(timeinfo->tm_min < 10) sprintf(minute, "0%d", timeinfo->tm_min);

	sprintf(second, "%d", timeinfo->tm_sec);
	if(timeinfo->tm_sec < 10) sprintf(second, "0%d", timeinfo->tm_sec);

    char content[SIZE];
    sprintf(content, "%s::%s%s%s-%s:%s:%s:%s::%s", level, day, month, year, hour, minute, second, command, desc);

    FILE* fptr_logs = fopen(logpath, "a");
    fprintf(fptr_logs, "%s\r\n", content);
    fclose(fptr_logs);
}

static int xmp_getattr(const char *path, struct stat *stbuf){
    put_systemlogs("INFO", "GETATTR", path);
    if(strstr(path, "/.") == NULL){
        sprintf(log_msg, "\t[~] Get Attribute : %s", path); logs();
    }else{
        return 0;
    }

    char path_fulldir[SIZE];
    sprintf(path_fulldir, "%s%s", dirpath, path);
    
    char realpath[SIZE];

    if(strcmp(path,"/") == 0){
        sprintf(realpath,"%s",dirpath);
    }else{
        sprintf(realpath, "%s%s", dirpath, path);

        if(strstr(path, "/AtoZ_")){
            char path_arr[SIZE]; sprintf(path_arr, "%s", path);
            char *after_atoz = strstr(path_arr, "/AtoZ_") + strlen("/AtoZ_");

            if(strstr(after_atoz, "/")){
                char *real_dir = getStrBetween(path, "/AtoZ_", "/");

                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");
                char *after_fullatoz = strstr(path, real_dir) + strlen(real_dir);
                char *dir_ext = get_filename_ext(after_fullatoz);

                if(strlen(dir_ext) > 0){
                    char path_to_decrypt[SIZE]; sprintf(path_to_decrypt, "%s", after_fullatoz);
                    path_to_decrypt[strlen(path_to_decrypt) - strlen(dir_ext) -1] = 0;
                    char *decrypted = aes_crypt(path_to_decrypt);

                    sprintf(realpath, "%s%s%s%s%s.%s", dirpath, before_atoz, "/", real_dir, decrypted, dir_ext);
                }else{
                    char *needDecrypt = strstr(path, real_dir) + strlen(real_dir);
                    char *decrypted = aes_crypt(needDecrypt);

                    sprintf(realpath, "%s%s%s%s%s", dirpath, before_atoz, "/", real_dir, decrypted);
                }
            }else{
                char *real_dir = after_atoz;
                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");
                sprintf(realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
            }
        }
    }

    // Biasanya habis buat file baru dia request get attribute ke dir yang keencrypt
    // Tapi folder yang diminta folder direktori barunya itu, maka
    // DIR* dir_check = opendir(realpath);
    
    if(access( realpath, F_OK ) != 0){
        sprintf(log_msg, "\t\t[~] dircheck not exist : %s", realpath); logs();

        // Get directory created
        char path_copy[SIZE]; sprintf(path_copy, "%s", realpath);
        char *end_str; char *dirname = strrev(strtok_r(strrev(path_copy), "/", &end_str));
        sprintf(log_msg, "\t\t[~] dirname : %s", dirname); logs();
        sprintf(log_msg, "\t\t[~] realpath : %s", realpath); logs();

        // get between
        int insubstr_idx = atoz_insubstr(realpath);
        if(insubstr_idx >= 0){
            // get between encrypted folder and dirname
            char *need_decrypt = getStrBetween(realpath, atoz_directory[insubstr_idx], dirname);
            sprintf(realpath, "%s%s%s", atoz_directory[insubstr_idx], aes_crypt(need_decrypt), dirname);
        }
    }

    sprintf(log_msg, "\t\t[~] Realpath to read is : %s", realpath); logs();

    int res = lstat(realpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    put_systemlogs("INFO", "READDIR", path);
    sprintf(log_msg, "[~] Dir readed : %s", path); logs();
    
    char path_fulldir[SIZE];
    sprintf(path_fulldir, "%s%s", dirpath, path);

    char realpath[SIZE];

    if(strcmp(path,"/") == 0){
        sprintf(realpath, "%s", dirpath);
    }else {
        sprintf(realpath, "%s%s", dirpath, path);

        if(strstr(path, "/AtoZ_")){
            char path_arr[SIZE]; sprintf(path_arr, "%s", path);
            char *after_atoz = strstr(path_arr, "/AtoZ_") + strlen("/AtoZ_");

            if(strstr(after_atoz, "/")){
                // Baca subdirektori
                char *real_dir = getStrBetween(path, "/AtoZ_", "/");
                char *needDecrypt = strstr(path, real_dir) + strlen(real_dir);
                char *decrypted = aes_crypt(needDecrypt);
                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");

                sprintf(realpath, "%s%s%s%s%s", dirpath, before_atoz, "/", real_dir, decrypted);
            }else{
                // Baca direktori sendiri
                char *real_dir = after_atoz;
                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");
                sprintf(realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
            }
        }
    }

    sprintf(log_msg, "\t[~] Reading realpath : %s", realpath); logs();

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(realpath);
    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        if( (!strcmp(de->d_name,".") || !strcmp(de->d_name,".."))){
            continue;
        }

        char dir_processed[SIZE];
        sprintf(dir_processed, "%s/%s", realpath, de->d_name);
        sprintf(log_msg, "\t[~] Dir Processed : %s", dir_processed); logs();
        
        struct stat st;

        if(atoz_contains(dir_processed)){
            sprintf(log_msg, "\t[~] Dir Processed : %s :: CONTAINS", dir_processed); logs();
            char atoz_name[SIZE];
            sprintf(atoz_name, "AtoZ_%s", de->d_name);
            strcpy(de->d_name, atoz_name);
        }

        if(strstr(path, "/AtoZ_")){
            if(de->d_type == DT_DIR){
                char encrypted_name[SIZE*2];
                sprintf(encrypted_name, "%s", aes_crypt(de->d_name));
                strcpy(de->d_name, encrypted_name);
                sprintf(log_msg, "\t\t[~] Dir Encrypted : %s", encrypted_name); logs();
            }else{
                // its a file
                // Lalu dapatkan ekstensi dari direktory yang diproses
                char *dir_ext = get_filename_ext(de->d_name);
                // printf("EXT :: %s\n", dir_ext);
                sprintf(log_msg, "\t\t[~] dir_ext : %s", dir_ext); logs();
                if(strlen(dir_ext) > 0){
                    // punya ekstensi, maka enkripsi sebelum ekstensi
                    char path_to_encrypt[SIZE*2]; sprintf(path_to_encrypt, "%s", de->d_name);
                    path_to_encrypt[strlen(path_to_encrypt) - strlen(dir_ext) -1] = 0;
                    sprintf(log_msg, "\t\t[~] To encrypt : %s", path_to_encrypt); logs();

                    char encrypted_name[SIZE*2];
                    sprintf(encrypted_name, "%s.%s", aes_crypt(path_to_encrypt), dir_ext);
                    strcpy(de->d_name, encrypted_name);
                    sprintf(log_msg, "\t\t[~] Dir Encrypted : %s", encrypted_name); logs();
                }else{
                    // tidak punya ekstensi
                    char path_to_encrypt[SIZE*2]; sprintf(path_to_encrypt, "%s", de->d_name);
                    sprintf(log_msg, "\t\t[~] To encrypt : %s", path_to_encrypt); logs();

                    char encrypted_name[SIZE*2];
                    sprintf(encrypted_name, "%s", aes_crypt(path_to_encrypt));
                    strcpy(de->d_name, encrypted_name);
                    sprintf(log_msg, "\t\t[~] Dir Encrypted : %s", encrypted_name); logs();
                }
            }
        }

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if(filler(buf, de->d_name, &st, 0)) break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    put_systemlogs("INFO", "READ", path);
    sprintf(log_msg, "[~] Readfile : %s", path); logs();

    char path_fulldir[SIZE];
    sprintf(path_fulldir, "%s%s", dirpath, path);
    char realpath[SIZE];

    if(strcmp(path,"/") == 0){
        sprintf(realpath, "%s", dirpath);
    }else {
        sprintf(realpath, "%s%s",dirpath,path);
        // check apakah dia atoz?
        if(strstr(path, "/AtoZ_")){
            char path_arr[SIZE]; sprintf(path_arr, "%s", path);
            char *after_atoz = strstr(path_arr, "/AtoZ_") + strlen("/AtoZ_");

            if(strstr(after_atoz, "/")){
                // Baca subdirektori
                char *real_dir = getStrBetween(path, "/AtoZ_", "/");
                
                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");
                char *after_fullatoz = strstr(path, real_dir) + strlen(real_dir);
                char *dir_ext = get_filename_ext(after_fullatoz);

                if(strlen(dir_ext) > 0){
                    char path_to_decrypt[SIZE]; sprintf(path_to_decrypt, "%s", after_fullatoz);
                    path_to_decrypt[strlen(path_to_decrypt) - strlen(dir_ext) -1] = 0;
                    char *decrypted = aes_crypt(path_to_decrypt);

                    sprintf(realpath, "%s%s%s%s%s.%s", dirpath, before_atoz, "/", real_dir, decrypted, dir_ext);
                }else{
                    char *needDecrypt = strstr(path, real_dir) + strlen(real_dir);
                    char *decrypted = aes_crypt(needDecrypt);

                    sprintf(realpath, "%s%s%s%s%s", dirpath, before_atoz, "/", real_dir, decrypted);
                }
            }else{
                // Baca direktori sendiri
                char *real_dir = after_atoz;
                char *before_atoz = getStrBetween(path_fulldir, dirpath, "/AtoZ_");
                sprintf(realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
            }
        }
    }

    int fd;
    int res;
    (void) fi;

    fd = open(realpath, O_RDONLY);
    if (fd == -1) return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    close(fd);

    return res;
}

/* Point 1. A
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode
*/
static int xmp_mkdir(const char *path, mode_t mode){
    put_systemlogs("INFO", "MKDIR", path);

    sprintf(log_msg, "[~] Makedir : %s", path); logs();
    char realpath[SIZE];
    
    // Get directory created
    char path_copy[SIZE]; sprintf(path_copy, "%s", path);
    char *end_str;
    char *dirname = strrev(strtok_r(strrev(path_copy), "/", &end_str));
    sprintf(log_msg, "[~] dirname : %s", dirname); logs();

    // Atoz_DIR
    char atoz_dir[SIZE]; sprintf(atoz_dir, "%s", dirname);
    atoz_dir[strlen("AtoZ_")] = 0;

    // Atoz_DIR
    char rx_dir[SIZE]; sprintf(rx_dir, "%s", dirname);
    rx_dir[strlen("RX_")] = 0;

    // Check AtoZ    
    if(!strcmp(atoz_dir, "AtoZ_")){
        // Get after atoz folder
        char path_arr[SIZE]; sprintf(path_arr, "%s", path);
        char *after_atoz = strstr(path_arr, "/AtoZ_") + strlen("/AtoZ_");
        sprintf(log_msg, "[~] Afteratoz : %s", after_atoz); logs();

        char full_atozfolder[SIZE];
        sprintf(full_atozfolder, "/AtoZ_%s", after_atoz);
        sprintf(log_msg, "\t\t[~] Fullatoz : %s", full_atozfolder); logs();
        sprintf(log_msg, "\t\t[~] Fullatoz len : %ld", strlen(full_atozfolder)); logs();

        // Get fullatoz tanpa "/"
        char full_atozfolder_noslash[SIZE];
        for(int i=1; i<strlen(full_atozfolder); i++){
            full_atozfolder_noslash[i-1] = full_atozfolder[i];
        }
        full_atozfolder_noslash[strlen(full_atozfolder)-1] = 0;
        sprintf(log_msg, "\t\t[~] full_atozfolder_noslash : %s", full_atozfolder_noslash); logs();

        // Dapatkan sebelum Atoz
        char path_arr2[SIZE]; sprintf(path_arr2, "%s", path);
        char *end_str;
        char *before_atoz = strtok_r(path_arr2, full_atozfolder_noslash, &end_str);
        sprintf(realpath, "%s%s%s", dirpath, before_atoz, after_atoz);
        atoz_insert(realpath);
        sprintf(log_msg, "%s --> %s%s", realpath, dirpath, path);
        put_logs(log_msg);
    }else if(!strcmp(rx_dir, "RX_")){
        sprintf(realpath, "%s%s", dirpath, path);
        rx_insert(realpath, ROT13);
        sprintf(log_msg, "rx dibuat dari mkdir : %s", realpath);
        put_logs(log_msg);
    }else{
        removeSubstr(path, "AtoZ_");
        char full_dir[SIZE]; sprintf(full_dir, "%s%s", dirpath, path);

        sprintf(log_msg, "\t\t[~] Clear Path : %s", full_dir); logs();
        int insubstr_idx = atoz_insubstr(full_dir);
        sprintf(log_msg, "\t\t[~] atoz_insubstr : %d", insubstr_idx); logs();
        if(insubstr_idx >= 0){
            // get between encrypted folder and dirname
            char *need_decrypt = getStrBetween(full_dir, atoz_directory[insubstr_idx], dirname);
            sprintf(log_msg, "\t\t[~] Need Decrypt : %s", need_decrypt); logs();
            if(need_decrypt == NULL){
                sprintf(realpath, "%s/%s", atoz_directory[insubstr_idx], dirname);
            }else{
                sprintf(realpath, "%s%s%s", atoz_directory[insubstr_idx], aes_crypt(need_decrypt), dirname);
            }
        }else{
            sprintf(realpath, "%s%s", dirpath, path);
        }
    }

    // Check apakah dir yang dibuat didalam folder RX
    char full_dir[SIZE]; sprintf(full_dir, "%s%s", dirpath, path);
    if(rx_insubstr(full_dir) >= 0 && !!strcmp(rx_dir, "RX_")){
        // Lalu encrypt direktory yang akan dibuat
        int THIS_CHYPER = rx_directory[rx_insubstr(full_dir)].CHIPER;
        char *encrypted;
        if(THIS_CHYPER == ROT13){
            encrypted = chaesarEncrypt(dirname, 13);
        }else if(THIS_CHYPER == VIGENERE){
            encrypted = vignereEncrypt(dirname, "SISOP");
        }

        char *before_dirname = getStrBetween(full_dir, dirpath, dirname);
        sprintf(realpath, "%s%s%s", dirpath, before_dirname, encrypted);
    }
    
    sprintf(log_msg, "\t\t[~] Final makedir : %s", realpath); logs();
    int res = mkdir(realpath, mode);
	if (res == -1) return -errno;
	return 0;
}

/* Point 1. B
Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
*/
static int xmp_rename(const char *from, const char *to){
    char temp_log[SIZE]; sprintf(temp_log, "%s::%s", from, to);
    put_systemlogs("INFO", "RENAME", temp_log);

    sprintf(log_msg, "[~] Rename : %s to %s", from, to); logs();

    char from_copy[SIZE]; sprintf(from_copy, "%s", from);
    char *end_str; char *dir_from = strrev(strtok_r(strrev(from_copy), "/", &end_str));
    char from_fulldir[SIZE]; sprintf(from_fulldir, "%s%s", dirpath, from);

    char to_copy[SIZE]; sprintf(to_copy, "%s", to);
    char *end_str2; char *dir_to = strrev(strtok_r(strrev(to_copy), "/", &end_str2));
    char to_fulldir[SIZE]; sprintf(to_fulldir, "%s%s", dirpath, to);

    char from_realpath[SIZE];
    char to_realpath[SIZE];
    sprintf(from_realpath, "%s%s", dirpath, from);
    sprintf(to_realpath, "%s%s", dirpath, to);

    bool from_atoz = false, to_atoz=false;
    char from_logs[SIZE]; sprintf(from_logs, "%s%s", dirpath, from);
    char to_logs[SIZE]; sprintf(to_logs, "%s%s", dirpath, to);

    if(strstr(from, "/AtoZ_")){
        char from_arr[SIZE]; sprintf(from_arr, "%s", from);
        char *after_atoz = strstr(from_arr, "/AtoZ_") + strlen("/AtoZ_");
        char *before_atoz;
        char *real_dir;

        if(strstr(after_atoz, "/")){
            // Mengganti subfoldernya /AtoZ_.../subdir
            // Get nama dirnya
            real_dir = getStrBetween(from, "/AtoZ_", "/");
            // Get antara real_dir dan dir_from
            char *needDecrypt = getStrBetween(from_fulldir, real_dir, dir_from);
            char *decrypted = aes_crypt(needDecrypt);

            // Get sebelum /AtoZ_
            before_atoz = getStrBetween(from_fulldir, dirpath, "/AtoZ_");

            char fix_path[SIZE];
            sprintf(fix_path, "%s%s%s%s%s", dirpath, before_atoz, "/", real_dir, decrypted);
            sprintf(log_msg, "\t[~] fix_path : %s", fix_path); logs();            
            
            sprintf(from_realpath, "%s%s", fix_path, aes_crypt(dir_from));
            sprintf(to_realpath, "%s%s", fix_path, dir_to);
        }else{
            // Mengganti folder /AtoZ_...
            real_dir = after_atoz;
            // Get sebelum /AtoZ_
            before_atoz = getStrBetween(from_fulldir, dirpath, "/AtoZ_");

            sprintf(from_realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
            sprintf(to_realpath, "%s%s%s%s", dirpath, before_atoz, "/", dir_to);
            // Lalu simpan ke dalam logs
            from_atoz = true;
        }

        if(from_atoz){
            sprintf(from_logs, "%s%s%s%s/%s", dirpath, before_atoz, "/AtoZ_", real_dir, dir_from);
        }else{
            sprintf(from_logs, "%s%s%s%s/%s", dirpath, before_atoz, "/", real_dir, dir_from);
        }
    }

    if(strstr(to, "/AtoZ_")){
        char to_arr[SIZE]; sprintf(to_arr, "%s", to);
        char *after_atoz = strstr(to_arr, "/AtoZ_") + strlen("/AtoZ_");
        char *before_atoz;
        char *real_dir;

        if(!strstr(after_atoz, "/")){
            // Mengganti menjadi folder /AtoZ_...
            real_dir = after_atoz;
            // Get sebelum /AtoZ_
            before_atoz = getStrBetween(to_fulldir, dirpath, "/AtoZ_");
            sprintf(to_realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);

            atoz_insert(to_realpath);
            // Lalu simpan ke dalam logs
            to_atoz = true;
        }

        if(to_atoz){
            sprintf(to_logs, "%s%s%s%s", dirpath, before_atoz, "/AtoZ_", real_dir);
        }else{
            sprintf(to_logs, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
        }
    }else if(strstr(to, "/RX_")){
        int last_rx = rx_insubstr(to_realpath);
        rx_insert(to_realpath, VIGENERE);
        sprintf(log_msg, "rx dibuat dari rename : %s", to_realpath);
        put_logs(log_msg);

        // Lalu secara rekursif ganti semua isi foldernya
        rename(from_realpath, to_realpath);

        if(last_rx >= 0){
            decryptRecursively(to_realpath, rx_directory[last_rx].CHIPER);
        }

        encryptRecursively(to_realpath, VIGENERE);
        return 0;
    }

    sprintf(log_msg, "[~] Fixed Rename : %s to %s", from_realpath, to_realpath); logs();
    rename(from_realpath, to_realpath);

    // Check apakah dia diganti dari /RX_ ke dir normal
    char rx_from[SIZE]; sprintf(rx_from, "%s", dir_from);
    rx_from[strlen("RX_")] = 0;

    char rx_to[SIZE]; sprintf(rx_to, "%s", dir_to);
    rx_to[strlen("RX_")] = 0;

    if(!strcmp(rx_from, "RX_") && !!strcmp(rx_to, "RX_")){
        int last_rx = rx_insubstr(from_realpath);
        if(last_rx >= 0){
            decryptRecursively(to_realpath, rx_directory[last_rx].CHIPER);
            strcpy(rx_directory[last_rx].DIR, "xxx..............xxx");
        }
    }

    if(from_atoz || to_atoz){
        sprintf(log_msg, "%s --> %s", from_logs, to_logs);
        put_logs(log_msg);
    }

	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .read       = xmp_read,
    .mkdir      = xmp_mkdir,
    .rename	    = xmp_rename,
};

int  main(int  argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
