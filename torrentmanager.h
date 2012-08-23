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

#pragma once

#include <string>
#include <boost/asio.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/session.hpp>

namespace IFVGrabber
{
	class TorrentManager
	{
		libtorrent::session *Session_;
		boost::asio::io_service IoService_;
		boost::asio::deadline_timer *Timer_;
	public:
		TorrentManager ();
		~TorrentManager ();

		void AddFile (const std::string& path);
		void AddDownload (const std::string& path);
		void ParseFile (const std::string& path);
		libtorrent::torrent_info GetTorrentInfo (const std::string& path);
		void QueryLibtorrentForWarnings (const boost::system::error_code& error);
	private:
		void Init ();
		bool IsTorrentFile (const std::string& path) const;
	};
}
