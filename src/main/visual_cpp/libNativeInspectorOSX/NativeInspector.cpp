/*
 * NativeInspector.cpp
 *
 *  Created on: 26.09.2025
 *      Author: thahnen
 */
#include <vector>
#include <string>
#include <fstream>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <netinet/in.h>

// Generated JNI C++ header
#include "com_hahnentt_rd_jni_NativeInspector.h"
#include "JNIUtil.h"

using namespace std;

/**
 *  Read the symbol table of the 64-Bit Mach-O (both x86_64 and ARM) binary and iterate over the
 *  exported methods, sorting out all the methods not related to JNI.
 *
 *  @param base content of the the file, moved towards the symbol table
 *  @param exports used to store JNI related methods by their name
 */
void parseMachOExports64(char *base, vector<string> &exports) {
  auto mh64 = (struct mach_header_64*) base;
  auto cmd = (struct load_command*) (base + sizeof(struct mach_header_64));

  for (uint32_t i = 0; i < mh64->ncmds; i++) {
    if (cmd->cmd == LC_SYMTAB) {
      auto symtab = (struct symtab_command*) cmd;
      const char *strtab = base + symtab->stroff;
      auto syms = (struct nlist_64*) (base + symtab->symoff);

      for (uint32_t j = 0; j < symtab->nsyms; j++) {
        const char *name = strtab + syms[j].n_un.n_strx;
        if (strncmp(name, "_Java_", 6) == 0) {
          exports.emplace_back(name);
        }
      }
    }
    cmd = (struct load_command*) ((char*) cmd + cmd->cmdsize);
  }
}

/**
 *  This is the macOS (aarch64) specific implementation of the following static method:
 *  - "String[] com.hahnentt.rd.jni.NativeInspektor.listExportedJniMethodsNative(String)"
 */
JNIEXPORT jobjectArray JNICALL Java_com_hahnentt_rd_jni_NativeInspector_listExportedJniMethodsNative(JNIEnv *env,
    jclass, jstring nativeFilePath) {
  // i) Try to open the native file and read the content
  const char *path = env->GetStringUTFChars(nativeFilePath, nullptr);
  ifstream file(path, ios::binary);
  if (!file.is_open()) {
    env->ReleaseStringUTFChars(nativeFilePath, path);
    throwUnsupportedOperationException(env, "Cannot open native file.");
    return nullptr;
  }

  // ii) Read the content into a buffer
  vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
  file.close();
  env->ReleaseStringUTFChars(nativeFilePath, path);

  char *base = buffer.data();

  // iii) Check the magic number of the file, sort out unsupported architecture
  uint32_t magic = *(uint32_t*) base;
  if (magic == FAT_CIGAM || magic == FAT_MAGIC) {
    auto fh = (struct fat_header*) base;
    auto arch = (struct fat_arch*) (base + sizeof(fat_header));

    bool found = false;
    for (uint32_t i = 0; i < ntohl(fh->nfat_arch); i++) {
      uint32_t cputype = ntohl(arch[i].cputype);
      if (cputype == CPU_TYPE_X86_64 || cputype == CPU_TYPE_ARM64) {
        base += ntohl(arch[i].offset);
        found = true;
        break;
      }
    }

    if (!found) {
      throwUnsupportedOperationException(env, "Unsupported architecture: 32-Bit x86/ARM is not supported.");
      return nullptr;
    }

    magic = *(uint32_t*) base;
  }

  // iv) Check the magic number of the file header, sort out unsupported architecture
  vector<string> exports;
  if (magic == MH_MAGIC_64 || magic == MH_CIGAM_64) {
    parseMachOExports64(base, exports);
    return convertVectorToJavaArray(env, exports);
  } else {
    throwUnsupportedOperationException(env, "Unsupported architecture: 32-Bit Mach-O is not supported.");
    return nullptr;
  }
}
