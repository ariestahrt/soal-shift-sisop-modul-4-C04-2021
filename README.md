# Kelompok C04 Modul 4 Sistem Operasi C
- I Kadek Agus Ariesta Putra 05111940000105
- Muhammad Arif Faizin 05111940000060
- Ahmad Lamaul Farid 05111940000134

## Deskripsi Soal
Link soal [Soal Shift 4](https://docs.google.com/document/d/1KlAG2thAMm42so8BZMylw216EETVGu_DAM5PfwJzl5U/edit)

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

## Soal 2
Pada soal no 2, diminta untuk membuat enkripsi tambahan dengan rincian sebagai berikut :
* Direktori yang dibuat dengan awalan "RX_[Nama]", akan menjadi terencode beserta isinya dengan tambahan algoritma ROT13, sehingga menjadi (Atbash + ROT13)
* Direktori yang direname dengan awalan "RX_[Nama]", akan menjadi terencode beserta isinya dengan tambahan Vignere Cipher dengan key "SISOP" (Case-sensitive, Atbash + Vignere)
* Direktori terencode yang direname dengan menghilangkan "RX_"nya, akan terdecode berdasar nama aslinya
* Setiap pembuatan direktori terencode (mkdir atau rename) otomatis tercatat pada catatan log beserta methodnya
* Direktori asli akan dipecah menjadi file-file kecil sebesar 1024 bytes. Namun akan tetap utuh apabila diakses dari filesystem rancangan. File akan dipecah dengan format `[Nama_File].[Ekstensi_File].[Nomor_Partisi]`

### A
Ketika melakukan perintah membuat direktori, maka akan melakukan checking apakah diawali dengan "RX_" atau tidak pada fungsi `xmp_mkdir()`.
```c
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

    // RX_DIR
    char rx_dir[SIZE]; sprintf(rx_dir, "%s", dirname);
    rx_dir[strlen("RX_")] = 0;
    ...

    // Check AtoZ    
    if(!strcmp(atoz_dir, "AtoZ_")){
        ...
    }else if(!strcmp(rx_dir, "RX_")){
        sprintf(realpath, "%s%s", dirpath, path);
        rx_insert(realpath, ROT13);
        sprintf(log_msg, "rx dibuat dari mkdir : %s", realpath);
        put_logs(log_msg);
    }else if(!strcmp(aisa_dir, "A_is_a_")){
        ...
    }else{
        ...
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
```

Kemudian jika terdapat "RX_" pada pathnya maka akan memanggil fungsi `rx_insert()` disertai dengan ROT13 sebagai enkripsi tambahannya. Fungsi rx_insert sendiri sebagai berikut :
```c
void rx_insert(char* dir, int chiper){
    sprintf(rx_directory[rx_last_idx].DIR, "%s", dir);
    rx_directory[rx_last_idx].CHIPER = chiper;
    rx_last_idx++;
}
```
Fungsi tersebut akan menyimpan directory dan jenis enkripsi yang akan digunakan pada folder tersebut.

Kemudian apakah direktori yang dibuat ada di dalam folder berawalan "RX_" atau tidak, dengan menggunakan fungsi `rx_insubstr()` sebagai berikut :
```c
int rx_insubstr(char *dir){
    int to_return = -1;
    for(int i=0; i<rx_last_idx; i++){
        if(strstr(dir, rx_directory[i].DIR)){
            to_return = i;
        }
    }
    return to_return;
}
```
Apabila direktori tersebut berada di dalam direktori berawalan "RX_", maka direktori tersebut akan dienkripsi sesuai dengan jenis enkripsinya masing-masing.

Untuk Chaesar Cipher menggunakan fungsi berikut :
```c
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
```

Sedangkan untuk Vignere Chip menggunakan fungsi berikut :
```c
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
```

### B dan C
Ketika melakukan perintah merename direktori, maka akan melakukan checking apakah diawali dengan "RX_" atau tidak pada fungsi `xmp_rename()`.

Apabila diawali dengan "RX_" maka direktori tersebut akan merename direktorinya dan mendecrypt secara rekursif semua direktori yang ada di dalamnya terlebih dahulu, kemudian dienkripsi ulang dengan Vignere Cipher.
```c
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

    ...

    if(strstr(to, "/AtoZ_")){
        ...
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
    }else if(strstr(to, "/A_is_a_")){
        a_is_a_insert(to_realpath);
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
    ...

	return 0;
}
```
Kemudian mengecek apakah merename dari direktori berawalan "RX_" menjadi normal atau tidak. Jika ya, maka decrypt direktori tersebut dan seluruh isinya secara rekursif.

Untuk fungsi decrypt secara rekursif sebagaia berikut :
```c
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
            sprintf(log_msg, "\t[~] decrypt, rename : %s to %s", from_rename, to_rename); logs();

            char next_path[SIZE];
            sprintf(next_path, "%s/%s", path, decrypted);
            decryptRecursively(next_path, CHIPER);
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
            sprintf(log_msg, "\t[~] decrypt, rename : %s to %s", from_rename, to_rename); logs();
        }
    }
    closedir(dir);
}
```

### D
Untuk melakukan logging, di `mkdir` ataupun `rename` maka akan memanggil fungsi `put_systemlogs()` dengan method `mkdir` atau `rename`.

### E
Untuk bagian soal ini, karena menurut kami penjelasannya ambigu dan sulit dipahami, maka kami memutuskan untuk tidak mengerjakannya. Karena di soal disebutkan jika file/folder yang memenuhi syarat seperti di bagian soal sebelumnya, akan dienkripsi sedangkan pada penjelasan soal ini tidak.

## Soal 3

## Soal 4
Pada soal no.4 ini kita diminta untuk membuat sebuah log system dengan spesifikasi sebagai berikut.
* Log system yang akan terbentuk bernama “SinSeiFS.log" pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem
* Log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
* Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
* Sisanya, akan dicatat pada level INFO.
* Format untuk logging yaitu: 
- [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]
- Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan
- INFO::28052021-10:00:00:CREATE::/test.txt
- INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt

```c
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
```
Kami membuat fungsi `put_systemlogs()` untuk mendapatkan format tanggal, bulan, tahun, jam, menit, detik yang diminta pada soal. Untuk log level WARNING (mencatat syscall rmdir dan unlink) pemanggilan fungsinya sebagai berikut: 
```c
// untuk rmdir
put_systemlogs("WARNING", "RMDIR", path);
// untuk unlink
put_systemlogs("WARNING", "UNLINK", path);
``` 
sedangkan untuk log level INFO (mencatat syscall selain rmdir dan unlink) pemanggilan fungsinya sebagai berikut:  
```c
// untuk syscall selain rmdir dan unlink
put_systemlogs("INFO", "GETATTR", path);
put_systemlogs("INFO", "READDIR", path);
put_systemlogs("INFO", "READ", path);
put_systemlogs("INFO", "MKDIR", path);
put_systemlogs("INFO", "RENAME", temp_log);
put_systemlogs("INFO", "WRITE", path);
put_systemlogs("INFO", "STATFS", path);
put_systemlogs("INFO", "CREATE", path);
```

## Kesulitan
Kesulitan yang kami temui pada praktikum kali ini adalah masih banyaknya soal yang susah untuk dipahami dan kontradiksi dengan soal lainnya. Sehingga dalam pengerjaannya kami kesulitan.
