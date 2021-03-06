/*
*  Copyright (C) 2014 Aurélien Rivet
*
*
*  This file is part of jdid tag editor.
*
*
*  jdid tag editor is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>
*
*
*/



#include "JTE.hpp"

using namespace std;

JTE::JTE(int argc, char **argv)
{
	parseCL(argc, argv);
}

JTE::~JTE()
{
	forward_list<AudioFile*>::iterator it;
	for(it = m_fileList.begin(); it != m_fileList.end(); ++it)
		AudioFileFactory::destroy((*it));
}

void JTE::parseCL(int argc, char **argv)
{
	try
	{
		TCLAP::CmdLine cmd("jdid tag editor", ' ', "0.1");

		TCLAP::ValueArg<string>
			cover("c", "cover", "Define the front cover tag", false, "", "path/to/cover", cmd),
			genre("g", "genre", "Define the genre tag", false, "", "genre", cmd),
			year("y", "year", "Define the year tag", false, "", "year", cmd),
			album("A", "album", "Define the album tag", false, "", "album", cmd),
			artist("a", "artist", "Define the artist tag", false, "", "artist", cmd),
			title("t", "title", "Define the title tag", false, "", "title", cmd),
			trackNumber("n", "tracknumber", "Define the tracknumber tag", false, "", "number", cmd);

		TCLAP::UnlabeledMultiArg<string> files("files", "The tag edition audio file(s) target(s)", true, "file(s)");
		cmd.add(files);

		//Run parsing
		cmd.parse(argc, argv);

		//Creating the file list
		int filesNumber = files.getValue().size();

		if(filesNumber == 0)
			throw TCLAP::ArgException("No files");

		for(vector<string>::const_iterator it = files.getValue().begin(); it != files.getValue().end(); ++it)
			m_fileList.push_front(AudioFileFactory::create((*it)));

		//Creating the tag edit map
		if(cover.isSet())
			m_tagList["cover"] = TagLib::String(cover.getValue(), TagLib::String::Type::UTF8);
		if(genre.isSet())
			m_tagList["genre"] = TagLib::String(genre.getValue(), TagLib::String::Type::UTF8);
		if(year.isSet())
			m_tagList["year"] = TagLib::String(year.getValue(), TagLib::String::Type::UTF8);
		if(album.isSet())
			m_tagList["album"] = TagLib::String(album.getValue(), TagLib::String::Type::UTF8);
		if(artist.isSet())
			m_tagList["artist"] = TagLib::String(artist.getValue(), TagLib::String::Type::UTF8);
		if(title.isSet())
			m_tagList["title"] = TagLib::String(title.getValue(), TagLib::String::Type::UTF8);
		if(trackNumber.isSet())
			m_tagList["trackNumber"] = TagLib::String(trackNumber.getValue(), TagLib::String::Type::UTF8);

		if(m_tagList.empty())
			throw TCLAP::ArgException("Nothing to do");
	}
	catch(TCLAP::ArgException &e)
	{
		cerr << "Error: " << e.error() << endl;
	}
	catch(string &s)
	{
		cerr << "Error: " << s << endl;
	}
}

void JTE::tagFiles()
{
	forward_list<AudioFile*>::iterator it;
	for(it = m_fileList.begin(); it != m_fileList.end(); ++it)
	{
		if((*it) == nullptr)
			continue;

		if(m_tagList.count("genre") != 0)
			(*it)->setGenre(m_tagList["genre"]);
		if(m_tagList.count("year") != 0)
			(*it)->setYear(m_tagList["year"]);
		if(m_tagList.count("album") != 0)
			(*it)->setAlbum(m_tagList["album"]);
		if(m_tagList.count("artist") != 0)
			(*it)->setArtist(m_tagList["artist"]);
		if(m_tagList.count("title") != 0)
			(*it)->setTitle(m_tagList["title"]);
		if(m_tagList.count("trackNumber") != 0)
			(*it)->setTrack(m_tagList["trackNumber"]);
		if(m_tagList.count("cover") != 0)
			(*it)->setCover(m_tagList["cover"]);

		(*it)->save();
	}
}

void JTE::run()
{
	tagFiles();
}
