#include "Util.h"

#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#endif


namespace vrlib
{
	namespace util
	{


		std::vector<std::string> split(std::string value, const std::string &seperator)
		{
			std::vector<std::string> ret;
			while (value.find(seperator) != std::string::npos)
			{
				int index = value.find(seperator);
				if (index != 0)
					ret.push_back(value.substr(0, index));
				value = value.substr(index + seperator.length());
			}
			ret.push_back(value);
			return ret;
		}

		std::vector<std::string> scandir(const std::string &path)
		{
			std::vector<std::string> files;
#ifndef WIN32
			DIR *dp;
			struct dirent *ep;
			dp = opendir(path.c_str());
			if (dp)
			{
				while ((ep = readdir(dp)))
				{
					if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
						continue;

					struct stat stFileInfo;
					stat((directory + "/" + ep->d_name).c_str(), &stFileInfo);

					if ((stFileInfo.st_mode & S_IFDIR))// && recursive)
					{
						/*vector<string> dirContents = getFiles(dir + "/" + ep->d_name, filter, recursive);
						for(unsigned int i = 0; i < dirContents.size(); i++)
						files.push_back(dirContents[i]);*/
						files.push_back(std::string(ep->d_name) + "/");
					}
					else
					{
						//						if(fnmatch(filter.c_str(), ep->d_name,0) == 0)
						files.push_back(ep->d_name);
					}
				}
				closedir(dp);
			}
			else
				Log::out << "Could not open directory '" << directory << "'" << Log::newline;
#else
			WIN32_FIND_DATA FileData;													// thingy for searching through a directory
			HANDLE hSearch;
			hSearch = FindFirstFile(std::string(path + "/*.*").c_str(), &FileData);
			if (hSearch != INVALID_HANDLE_VALUE)										// if there are results...
			{
				while (true)														// loop through all the files
				{
					if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
					{
						files.push_back(std::string(FileData.cFileName) + "/");
					}
					else
						files.push_back(FileData.cFileName);

					if (!FindNextFile(hSearch, &FileData))								// find next file in the resultset
					{
						if (GetLastError() == ERROR_NO_MORE_FILES)						// we're finished when there are no more files
							break;
						else
							return files;													// wow, something really weird happened
					}
				}
				FindClose(hSearch);
			}
#endif	
			return files;
		}

	}

}