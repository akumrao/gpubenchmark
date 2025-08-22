/* This file is part of mediaserver. A webrtc sfu server.
 * Copyright (C) 2018 Arvind Umrao <akumrao@yahoo.com> & Herman Umrao<hermanumrao@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */


#include "base/filesystem.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/util.h"
#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>


namespace base {
    namespace fs {


        static const char* separatorWin = "\\";
        static const char* separatorUnix = "/";
#ifdef base_WIN
        const char delimiter = '\\';
const char* separator = separatorWin;
static const char* sepPattern = "/\\";
#else
        const char delimiter = '/';
        const char* separator = separatorUnix;
        static const char* sepPattern = "/";
#endif


        std::string filename(const std::string& path)
        {
            size_t dirp = path.find_last_of(fs::sepPattern);
            if (dirp == std::string::npos)
                return path;
            return path.substr(dirp + 1);
        }


        std::string dirname(const std::string& path)
        {
            size_t dirp = path.find_last_of(sepPattern);
            if (dirp == std::string::npos)
                return ".";
            return path.substr(0, dirp);
        }


        std::string basename(const std::string& path)
        {
            size_t dotp = path.find_last_of(".");
            if (dotp == std::string::npos)
                return path;

            size_t dirp = path.find_last_of(fs::sepPattern);
            if (dirp != std::string::npos && dotp < dirp)
                return path;

            return path.substr(0, dotp);
        }


        std::string extname(const std::string& path, bool includeDot)
        {
            size_t dotp = path.find_last_of(".");
            if (dotp == std::string::npos)
                return "";

            // Ensure the dot was not part of the pathname
            size_t dirp = path.find_last_of(fs::sepPattern);
            if (dirp != std::string::npos && dotp < dirp)
                return "";

            return path.substr(dotp + (includeDot ? 0 : 1));
        }


        bool exists(const std::string& path)
        {
// Normalize is needed to ensure no
// trailing slash for directories or
// stat fails to recognize validity.
// // TODO: Do we need transcode here?
//  #ifdef base_WIN
//      struct _stat s;
//      return _stat(fs::normalize(path).c_str(), &s) != -1;
//  #else
//      struct stat s;
//      return stat(fs::normalize(path).c_str(), &s) != -1;
//  #endif


            if(!access(path.c_str(), F_OK )){
                return true;
            }


            return false;
        }


        bool isdir(const std::string& path)
        {

            exit(0);

// TODO: Do we need transcode here?
// #ifdef base_WIN
//     struct _stat s;
//     _stat(fs::normalize(path).c_str(), &s);
// #else
//     struct stat s;
//     stat(fs::normalize(path).c_str(), &s);
// #endif
//     // S_IFDIR: directory file.
//     // S_IFCHR: character-oriented device file
//     // S_IFBLK: block-oriented device file
//     // S_IFREG: regular file
//     // S_IFLNK: symbolic link
//     // S_IFSOCK: socket
//     // S_IFIFO: FIFO or pipe
//     return (s.st_mode & S_IFDIR) != 0;

            return false;
        }


        std::int64_t filesize(const std::string& path)
        {
// #ifdef base_WIN
//     struct _stat s;
//     if (_stat(path.c_str(), &s) == 0)
// #else
//     struct stat s;
//     if (stat(path.c_str(), &s) == 0)
// #endif
//         return s.st_size;
            return -1;
        }

        void mkdir(const std::string& path, int mode)
        {
            //internal::FSapi(mkdir, path.c_str(), mode)
            mkdir( path.c_str(), mode);
        }

        void mkdirr(const std::string& path, int mode)
        {
            std::string current;
            std::string level;
            std::istringstream istr(fs::normalize(path));

            while (std::getline(istr, level, fs::delimiter)) {
                if (level.empty())
                    continue;

#ifdef base_WIN
                current += level;
        if (level.at(level.length() - 1) == ':') {
            current += fs::separator;
            continue; // skip drive letter
        }
#else
                if (current.empty())
                    current += fs::separator;
                current += level;
#endif
                // create current level
                if (!fs::exists(current))
                    fs::mkdir(current.c_str(), mode); // create or throw

                current += fs::separator;
            }
        }



        void trimslash(std::string& path)
        {
            if (path.empty()) return;
            size_t dirp = path.find_last_of(sepPattern);
            if (dirp == path.length() - 1)
                path.resize(dirp);
        }


        std::string normalize(const std::string& path)
        {
            std::string s(util::replace(path,
#ifdef base_WIN
                    separatorUnix, separatorWin
#else
                                        separatorWin, separatorUnix
#endif
            ));

            // Trim the trailing slash for stat compatibility
            trimslash(s);
            return s;
        }



        void addsep(std::string& path)
        {
            if (!path.empty() && path.at(path.length() - 1) != fs::separator[0])
                path.append(fs::separator, 1);
        }


        void addnode(std::string& path, const std::string& node)
        {
            fs::addsep(path);
            path += node;
        }


        bool savefile(const std::string& path, const char* data, size_t size,
                      bool whiny)
        {
            std::ofstream ofs(path, std::ios_base::binary | std::ios_base::out);
            if (ofs.is_open())
                ofs.write(data, size);
            else {
                if (whiny)
                    throw std::runtime_error("Cannot save file: " + path);
                return false;
            }
            return true;
        }


    } // namespace fs
} // namespace base


