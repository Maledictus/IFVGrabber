/*
 *  Copyright (c) 2012 Oleg Linkin MaledictusDeMagog@gmail.com
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <boost/program_options.hpp>
#include "torrentmanager.h"

int main (int argc, char *argv [])
{
	boost::program_options::options_description description ("Allowed options");
	description.add_options ()
		("help,h", "show this message.")
		("torrent,t", boost::program_options::value<std::string> (),
				"path to torrent file.")
		("file,f", boost::program_options::value<std::string> (),
				"relative path to file in torrent.")
		("save,s", boost::program_options::value<std::string> (),
				"directory to save images. Should exist.");

	boost::program_options::variables_map vm;
	boost::program_options::command_line_parser opts =
			boost::program_options::command_line_parser (argc, argv)
					.options (description)
					.allow_unregistered ();

	try
	{
		boost::program_options::store (opts.run (), vm);
		boost::program_options::notify (vm);
	}
	catch (const boost::program_options::invalid_command_line_syntax& e)
	{
		std::cout << e.what () << std::endl;
		return 1;
	}

	if (vm.count ("help"))
	{
		std::cout << description << std::endl;
		return 1;
	}

	std::string torrent, file, path;
	if (vm.count ("torrent"))
		torrent = vm ["torrent"].as<std::string> ();
	if (vm.count ("file"))
		file = vm ["file"].as<std::string> ();
	if (vm.count ("save"))
		path = vm ["save"].as<std::string> ();

	IFVGrabber::TorrentManager *manager = new IFVGrabber::TorrentManager;
	manager->AddFile (torrent, file, path);

	char a;
	std::cin.unsetf(std::ios_base::skipws);
	std::cin >> a;

	return 0;
}
