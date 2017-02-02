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
			return hsv2rgb(glm::vec3((rand() / (float)RAND_MAX) * 360, 1, 1));
		}


		glm::vec3 hsv2rgb(glm::vec3 hsv)
		{
			double      hh, p, q, t, ff;
			long        i;

			if (hsv.y <= 0.0) {       // < is bogus, just shuts up warnings
				return glm::vec3(hsv.z, hsv.z, hsv.z);
			}
			hh = hsv.x;
			while (hh >= 360.0) hh = 0.0;
			hh /= 60.0;
			i = (long)hh;
			ff = hh - i;
			p = hsv.z * (1.0 - hsv.y);
			q = hsv.z * (1.0 - (hsv.y * ff));
			t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

			switch (i) {
			case 0:
				return glm::vec3(hsv.z, t, p);
			case 1:
				return glm::vec3(q, hsv.z, p);
			case 2:
				return glm::vec3(p, hsv.z, t);
			case 3:
				return glm::vec3(p, q, hsv.z);
			case 4:
				return glm::vec3(t, p, hsv.z);
			case 5:
			default:
				return glm::vec3(hsv.z, p, q);
			}
		}


		glm::vec3 rgb2hsv(glm::vec3 rgb)
		{
			glm::vec3         out;
			float      min, max, delta;

			min = rgb.r < rgb.g ? rgb.r : rgb.g;
			min = min  < rgb.b ? min : rgb.b;

			max = rgb.r > rgb.g ? rgb.r : rgb.g;
			max = max  > rgb.b ? max : rgb.b;

			out.z = max;                                // v
			delta = max - min;
			if (max > 0.0) {
				out.y = (delta / max);                  // s
			}
			else {
				// r = g = b = 0                        // s = 0, v is undefined
				out.y = 0.0;
				out.x = NAN;                            // its now undefined
				return out;
			}
			if (rgb.r >= max)                           // > is bogus, just keeps compilor happy
				out.x = (rgb.g - rgb.b) / delta;        // between yellow & magenta
			else
				if (rgb.g >= max)
					out.x = 2.0f + (rgb.b - rgb.r) / delta;  // between cyan & yellow
				else
					out.x = 4.0f + (rgb.r - rgb.g) / delta;  // between magenta & cyan

			out.x *= 60.0;                              // degrees

			if (out.x < 0.0)
				out.x += 360.0;

			return out;
		}

		float random(float min, float max)
		{
			return (float)((double)rand() / (RAND_MAX + 1)) * (max - min + 1) + min;
		}

		std::string getGuid()
		{
#ifdef WIN32
			UUID uuid = { 0 };
			::UuidCreate(&uuid);
			unsigned char* cuuid = nullptr;
			::UuidToString(&uuid, &cuuid);
			std::string guid = std::string((char*)cuuid);
			::RpcStringFree(&cuuid);
			return guid;
#endif
			return "";
		}

	}

}