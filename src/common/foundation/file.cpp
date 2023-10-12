#include "foundation/file.h"

#include "foundation/memory.h"
#include "foundation/assert.h"
#include "foundation/string.h"

#if defined(_WIN64)
#include <windows.h>
#else
#define MAX_PATH 65536
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <string.h>

namespace Engine
{
	static long file_get_size( FileHandle f )
	{
		long fileSizeSigned;

		fseek(f, 0, SEEK_END);
		fileSizeSigned = ftell( f );
		fseek( f, 0, SEEK_SET );

		return fileSizeSigned;
	}


	void file_directory_from_path( char* path )
	{
		char* last_point = strrchr( path, '.' );
		char* last_seperator = strrchr(path, '/');
		if ( last_seperator != nullptr && last_point > last_seperator )
		{
			*(last_seperator + 1) = 0;
		}
		else
		{
			// Try searching backslash.
			last_seperator = strchr( path, '\\' );
			if( last_seperator != nullptr && last_point > last_seperator )
			{
				*( last_seperator + 1 ) = 0;
			}
			else
			{
				// Wrong input.
				RASSERTM( false, "Malformed path %s!", path );
			}
		}
	}

	void file_name_from_path(char* path)
	{
		char* last_seperator = strrchr( path, '/' );
		if ( last_seperator == nullptr )
		{
			last_seperator = strrchr( path, '\\' );
		}

		if ( last_seperator != nullptr )
		{
			sizet name_length = strlen( last_seperator + 1 );

			memcpy( path, last_seperator + 1, name_length );
			path[ name_length ] = 0;
		}
	}

	char* file_externsion_from_path( char* path )
	{
		char* last_separator = strrchr( path, '.' );

		return last_separator + 1;
	}

	bool file_exists(cstring path)
	{
#if defined(_WIN64)
		WIN32_FILE_ATTRIBUTE_DATA unused;
		return GetFileAttributesExA(path, GetFileExInfoStandard, &unused);
#else
		int result = access(path, F_OK);
		return ( result == 0 );
#endif // _WIN64
	}

	//
	bool file_delete(cstring path)
	{
#if defined(_WIN64)
		int result = remove( path );
		return result != 0;
#else
		int result = remove( path );
		return (result == 0);
#endif // _WIN64
	}

	//
	void directory_current( Directory* directory )
	{
#if defined(_WIN64)
		DWORD written_chars = GetCurrentDirectoryA( k_max_path, directory->path );
		directory->path[ written_chars ] = 0;
#else
		getcwd( directory->path, k_max_path );

#endif // _WIN64
	}

	void directory_change(cstring path)
	{
#if defined( _WIN64 )
		if( !SetCurrentDirectoryA( path ) )
		{
			rprint( "Cannot chnage current directory to %s\n", path );
		}
#else
		if (chdir(path) != 0)
		{
			rprint( "Cannot Change current directory to %s\n", path );
		}	
#endif // _WIN64
	}

	void environment_variable_get(cstring name, char* output, u32 output_size)
	{
#if defined(_WIN64)
		ExpandEnvironmentStringsA( name, output, output_size );
#else
		cstring real_output = getenv( name );
		strncpy( output, real_output, output_size );
#endif
	}

	char* file_read_binary(cstring filename, Allocator* allocator, sizet* size)
	{
		char* out_data = 0;

		FILE* file = fopen( filename, "rb" );

		if ( file )
		{
			// TODO : Use filesize or read result ?
			sizet filesize = file_get_size( file );

			out_data = (char*)ralloca( filesize + 1, allocator );
			fread( out_data, filesize, 1, file );
			out_data[filesize] = 0;

			if ( size )
				*size = filesize;

			fclose( file );
		}

		return out_data;
	}

	char* file_read_text(cstring filename, Allocator* allocator, sizet* size)
	{
		char* text = 0;

		FILE* file = fopen( filename, "r" );

		if ( file )
		{
			sizet filesize = file_get_size( file );
			text = (char*)ralloca( filesize + 1, allocator );
			// Correct: use elementcount as filesize, bytes_read becomes the actual bytes read
			// AFTER the end of line conversion for Windows (it uses \r\n).
			sizet bytes_read = fread( text, 1, filesize, file );
			text[ bytes_read ] = 0;

			if ( size )
				*size = filesize;

			fclose( file );
		}

		return text;
	}

	FileReadResult file_read_binary(cstring filename, Allocator* allocator)
	{
		FileReadResult result { nullptr, 0 };

		FILE* file = fopen( filename, "rb");

		if ( file )
		{
			// TODO: Use filesize or read result ?
			sizet filesize = file_get_size(file);

			result.data = ( char* )ralloca( filesize, allocator );
			fread( result.data, filesize, 1, file );
			
			result.size = filesize;

			fclose(file);
		}

		return result;
	}
	
	FileReadResult file_read_text( cstring filename, Allocator* allocator )
	{
		FileReadResult result{ nullptr, 0 };

		FILE* file = fopen(filename, "r");

		if ( file )
		{
			sizet filesize = file_get_size( file );
			result.data = ( char* )ralloca( filesize + 1, allocator );
			// Correct: use elementcount as filesize, bytes_read beacome the actual bytes read
			// AFTER the end of line conversion for Windows (it uses \r\n).
			sizet bytes_read = fread( result.data, 1, filesize, file );

			result.data[ bytes_read ] = 0;

			result.size = bytes_read;

			fclose(file);
		}

		return result;
	}

	void file_write_binary(cstring filename, void* memory, sizet size)
	{
		FILE* file = fopen( filename, "wb" );
		fwrite( memory, size, 1, file );
		fclose( file );
	}

} // namespace Engine.