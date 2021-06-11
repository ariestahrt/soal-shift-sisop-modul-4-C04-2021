# Kelompok C04 Modul 4 Sistem Operasi C
- I Kadek Agus Ariesta Putra 05111940000105
- Muhammad Arif Faizin 05111940000060
- Ahmad Lamaul Farid 05111940000134

## Deskripsi Soal
Pada soal modul 4 ini kita diminta untuk membuat file system dengan ketentuan masing-masing soal sebagai berikut

## Soal 1
Pada soal nomor 1, kita diminta untuk membuat file system dengan folder yang terenkripsi apabila memiliki awalan "AtoZ_" ketentuan sebagai berikut:

### A
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.

Pertama-tama kami membuat array untuk menyimpan direktory yang terkenripsi dengan fungsi-fungsi sebagai berikut:
```c
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
```

Pada fungsi mkdir adalah sebagai berikut:
```c
static int xmp_mkdir(const char *path, mode_t mode){
    // ...
    char realpath[SIZE];
    
    // Get directory created
    char path_copy[SIZE]; sprintf(path_copy, "%s", path);
    char *end_str;
    char *dirname = strrev(strtok_r(strrev(path_copy), "/", &end_str));
    sprintf(log_msg, "[~] dirname : %s", dirname); logs();

    // Atoz_DIR
    char atoz_dir[SIZE]; sprintf(atoz_dir, "%s", dirname);
    atoz_dir[strlen("AtoZ_")] = 0;
    // ...

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
    }
    // ...
    else{
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

    // ...
}
```

Jadi asumsi yang kami gunakan adalah apabila kita membuat folder "AtoZ_" melalui filesystem, maka di dalam direktory "~/Download" tidak akan terenkripsi beserta isinya. File/folder akan terenkripsi apabila dibuat melalui filesystem dan dilihat melalui filesystem. Untuk menunjang hal tersebut pada fungsi getattr kami gunakan memiliki potongan kode sebagai berikut:

```c
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
```

### B
Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.

Poin B disini terlibat dalam fungsi rename, dengan potongan kode sebagai berikut:
```c
static int xmp_rename(const char *from, const char *to){
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
    }
}
```

### C
Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

Poin ini terlibat di fungsi rename, apabila diubah menjadi direktori tidak terenkripsi pada potongan kode sebagai berikut:

```c
    // Mengganti folder /AtoZ_...
    real_dir = after_atoz;
    // Get sebelum /AtoZ_
    before_atoz = getStrBetween(from_fulldir, dirpath, "/AtoZ_");

    sprintf(from_realpath, "%s%s%s%s", dirpath, before_atoz, "/", real_dir);
    sprintf(to_realpath, "%s%s%s%s", dirpath, before_atoz, "/", dir_to);
    // Lalu simpan ke dalam logs
    from_atoz = true;
```

### D
Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]

Untuk poin ini, kami membuat fungsi put_logs untuk menyimpan logs ke dalam file logs.txt sebagai berikut:

```c
void put_logs(char *message){
    char log_file[SIZE];
    sprintf(log_file, "%s/logs.txt", dirpath);

    printf("PUTS LOGS CALLED----\n");
    printf("LOGS FILE :: %s\n", log_file);
    
    FILE* fptr_logs = fopen(log_file, "a");
    fprintf(fptr_logs, "%s\r\n", message);
    fclose(fptr_logs);
}
```

dan dipanggil pada fungsi rename sebagai berikut:
```c
    if(from_atoz || to_atoz){
        sprintf(log_msg, "%s --> %s", from_logs, to_logs);
        put_logs(log_msg);
    }
```


### E
Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

Untuk poin ini terdapat pada fungsi getattr pada poin nomor 1.

Sehingga implementasi lengkap dapat dilihat pada file [SinSeiFS_C04.c](https://github.com/ariestahrt/soal-shift-sisop-modul-4-C04-2021/blob/master/SinSeiFS_C04.c)