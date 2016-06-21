#include "util.h"
#include "Log.h"
using vrlib::Log;

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
					stat((path + "/" + ep->d_name).c_str(), &stFileInfo);

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
				logger << "Could not open directory '" << path << "'" << Log::newline;
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

		glm::vec3 randomHsv()
		{
			glm::vec3 in((rand() / (float)RAND_MAX) * 360, 1, 1);
			glm::vec3 out;
			float      hh, p, q, t, ff;
			long        i;

			if (in.y <= 0.0) {       // < is bogus, just shuts up warnings
				out.r = in.z;
				out.g = in.z;
				out.b = in.z;
				return out;
			}
			hh = in.x;
			if (hh >= 360.0) hh = 0.0;
			hh /= 60.0;
			i = (long)hh;
			ff = hh - i;
			p = in.z * (1.0f - in.y);
			q = in.z * (1.0f - (in.y * ff));
			t = in.z * (1.0f - (in.y * (1.0 - ff)));

			switch (i) {
			case 0:
				out.r = in.z;
				out.g = t;
				out.b = p;
				break;
			case 1:
				out.r = q;
				out.g = in.z;
				out.b = p;
				break;
			case 2:
				out.r = p;
				out.g = in.z;
				out.b = t;
				break;

			case 3:
				out.r = p;
				out.g = q;
				out.b = in.z;
				break;
			case 4:
				out.r = t;
				out.g = p;
				out.b = in.z;
				break;
			case 5:
			default:
				out.r = in.z;
				out.g = p;
				out.b = q;
				break;
			}
			return out;
		}

	}

}