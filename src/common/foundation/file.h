#pragma once

#include "foundation/platform.h"
#include <stdio.h>

namespace Engine
{
	struct Allocator;
	struct StringArray;

#if defined(_WIN64)

	typedef struct __FILETIME
	{
		unsigned long		dwLowDateTime;
		unsigned long		dwHighDateTime;
	} FILETIME, * PFILETIME, * LPFILETIME;

	using FileTime = __FILETIME;

#endif

	using FileHandle = FILE*;

	static const u32		k_max_path = 512;

	//
	//
	struct Directory
	{
		char				path[ k_max_path ];

#if defined (_WIN64)
		void* os_handle;
#endif
	}; // struct Directory

	struct FileReadResult
	{
		char*				data;
		sizet				size;
	};

	// Read file and allocate memory from allocator.
	// User is responsible for freeing the memory.
	char*								file_read_binary( cstring filename, Allocator* allocator, sizet* size );
	char*								file_read_text( cstring filename, Allocator* allocator, sizet* size );

	FileReadResult						file_read_binary( cstring filename, Allocator* allocator );
	FileReadResult						file_read_text( cstring filename, Allocator* allocator );

	bool								file_exists( cstring path );
	bool								file_delete( cstring path );

	// Inplace path methods.
	void								file_directory_from_path( char* path );				// Retrieve path without the filename. Path is a preallocated string buffer. It moves the terminator before the name of the file.
	void								file_name_from_path( char* path );
	void								directory_current( Directory* directory );

	void								directory_current( Directory* directory );
	void								directory_change( cstring path );
	
	

	// TODO: move
	void								environment_variable_get( cstring name, char* output, u32 output_size );

	struct ScopedFile
	{
		ScopedFile( cstring filename, cstring mode );
		~ScopedFile();

		FileHandle						file;
	}; // struct ScopedFile

} // namespace Engine