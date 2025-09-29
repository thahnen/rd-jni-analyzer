/*
* NativeInspector.cpp
*
*  Created on: 29.09.2025
*      Author: thahnen
*/

#include <windows.h>
#include <vector>
#include <string>

// Generated JNI C++ header
#include "com_hahnentt_rd_jni_NativeInspector.h"
#include "JNIUtil.h"

using namespace std;

/// <summary>
/// This converts the Relative Virtual Address (RVA) to an offset inside the file data using the
/// sections and their virtual address.
/// </summary>
/// <param name="rva">Relative Virtual Address to find the offset to using the sections</param>
/// <param name="sections">Pointer to the sections inside the PE file</param>
/// <param name="numSections">Number of sections inside the PE file</param>
DWORD RvaToOffset(DWORD rva, const IMAGE_SECTION_HEADER* sections, WORD numSections) {
	for (DWORD i = 0; i < numSections; i++) {
		auto section = sections[i];

		DWORD sectionStart = section.VirtualAddress;
		DWORD sectionEnd = sectionStart + section.Misc.VirtualSize;
		if (rva >= sectionStart && rva < sectionEnd) {
			return (rva - sectionStart + section.PointerToRawData);
		}
	}
	return 0;
}

/// <summary>
/// This parses the export table of the 32-/64-Bit PE file and iterates over the exported names,
/// sorting out all the methods not related to JNI.
/// </summary>
/// <param name="data">The binary data of the PE file</param>
/// <param name="exportRva">Relative Virtual Address of the export table</param>
/// <param name="sections">Pointer to the sections inside the PE file</param>
/// <param name="numSections">Number of sections inside the PE file</param>
void parsePEExports(BYTE* data, DWORD exportRva, const IMAGE_SECTION_HEADER* sections, WORD numSections,
	vector<string> &exports) {
	DWORD exportOffset = RvaToOffset(exportRva, sections, numSections);
	if (exportOffset == 0) {
		return;
	}

	auto exportDir = (IMAGE_EXPORT_DIRECTORY*)(data + exportOffset);
	auto nameRvas = (DWORD*)(data + RvaToOffset(exportDir->AddressOfNames, sections, numSections));

	for (DWORD i = 0; i < exportDir->NumberOfNames; i++) {
		DWORD nameOffset = RvaToOffset(nameRvas[i], sections, numSections);
		if (nameOffset == 0) {
			continue;
		}

		auto name = (char*)(data + nameOffset);
		if (strncmp(name, "Java_", 5) == 0) {
			exports.emplace_back(name);
		}
	}
}

/// <summary>
/// This is the Windows (x86_64) specific implementation of the following static method:
/// - "String[] com.hahnentt.rd.jni.NativeInspektor.listExportedJniMethodsNative(String)"
/// </summary>
JNIEXPORT jobjectArray JNICALL Java_com_hahnentt_rd_jni_NativeInspector_listExportedJniMethodsNative(JNIEnv* env,
	jclass, jstring nativeFilePath) {
	// i) Try to open the native file and read the content
	const char *path = env->GetStringUTFChars(nativeFilePath, nullptr);

	FILE* file;
	if (fopen_s(&file, path, "rb") != 0) {
		env->ReleaseStringUTFChars(nativeFilePath, path);
		throwUnsupportedOperationException(env, "Cannot open native file.");
		return nullptr;
	}

	// ii) Read the content into a buffer
	fseek(file, 0, SEEK_END);
	auto size = ftell(file);
	fseek(file, 0, SEEK_SET);
	vector<BYTE> buffer(size);
	fread(buffer.data(), 1, size, file);
	fclose(file);
	env->ReleaseStringUTFChars(nativeFilePath, path);

	auto base = (IMAGE_DOS_HEADER*)buffer.data();

	// iii) Check the PE header and NT signature of the file
	if (base->e_magic != IMAGE_DOS_SIGNATURE) {
		throwUnsupportedOperationException(env, "Invalid PE header / magic number (DOS).");
		return nullptr;
	}

	auto nt32 = (IMAGE_NT_HEADERS32*)(buffer.data() + base->e_lfanew);
	if (nt32->Signature != IMAGE_NT_SIGNATURE) {
		throwUnsupportedOperationException(env, "Invalid PE header / magic number (NT).");
		return nullptr;
	}

	auto nt64 = (IMAGE_NT_HEADERS64*)(buffer.data() + base->e_lfanew);
	IMAGE_SECTION_HEADER* sections = nullptr;
	WORD numSections = 0;
	DWORD exportRva = 0;

	// iv) Sort out unsupported architecture
	switch (nt32->FileHeader.Machine) {
	case IMAGE_FILE_MACHINE_I386:
		sections = IMAGE_FIRST_SECTION(nt32);
		numSections = nt32->FileHeader.NumberOfSections;
		exportRva = nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		break;
	case IMAGE_FILE_MACHINE_AMD64:
		sections = IMAGE_FIRST_SECTION(nt64);
		numSections = nt64->FileHeader.NumberOfSections;
		exportRva = nt64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		break;
	default:
		throwUnsupportedOperationException(env, "Unsupported architecture: 32-/64-Bit ARM is not supported.");
		return nullptr;
	}

	// v) Make sure Relative Virtual Address to export table and information about sections exists
	if (exportRva == 0 || sections == nullptr) {
		throwUnsupportedOperationException(env, "Export table (RVA) or section information not found.");
		return nullptr;
	}
	
	// vi) Parse all the exported names and return results
	vector<string> exports;
	parsePEExports(buffer.data(), exportRva, sections, numSections, exports);
	return convertVectorToJavaArray(env, exports);
}
