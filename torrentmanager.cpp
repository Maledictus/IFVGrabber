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


#include <fstream>
#include <boost/filesystem.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include "torrentmanager.h"

namespace IFVGrabber
{
	TorrentManager::TorrentManager ()
	{
		Init ();

		Timer_ = new boost::asio::deadline_timer (IoService_);
		Timer_->expires_from_now (boost::posix_time::seconds (2));
	}

	TorrentManager::~TorrentManager ()
	{
		Timer_->cancel ();
		IoService_.stop ();
		delete Timer_;
		delete Session_;
	}

	void TorrentManager::AddFile (const std::string& path)
	{
		IsTorrentFile (path) ?
			AddDownload (path) :
			ParseFile (path);
	}

	void TorrentManager::AddDownload(const std::string& path)
	{
		libtorrent::torrent_handle handle;

		std::string savePath = boost::filesystem::absolute ("files").string ();
		try
		{
			libtorrent::add_torrent_params atp;
			atp.ti = new libtorrent::torrent_info (GetTorrentInfo (path));
			atp.auto_managed = false;
			atp.storage_mode = libtorrent::storage_mode_sparse;
			atp.paused = false;
			atp.save_path = savePath;
			atp.duplicate_is_error = true;
			handle = Session_->add_torrent (atp);
		}
		catch (const libtorrent::libtorrent_exception& e)
		{
// 			Core::Instance().HandleLibtorrentException (e);
			std::cout << "-1" << e.what () << e.error ().value () << std::endl;
			return;
		}
		catch (const std::runtime_error& e)
		{
			std::cout << "Runtime error" << std::endl;
			return;
		}
		handle.set_sequential_download (true);
		handle.resume ();

		Timer_->async_wait (boost::bind (&TorrentManager::QueryLibtorrentForWarnings, this, _1));
		IoService_.run ();
	}

	void TorrentManager::ParseFile(const std::string& path)
	{
		std::ifstream file;
		file.open (path, std::ifstream::in);
		std::string pathToTorrent;
		while (!file.eof ())
		{
			file >> pathToTorrent;
			if (IsTorrentFile (pathToTorrent))
				AddDownload (pathToTorrent);
		}
	}

	libtorrent::torrent_info IFVGrabber::TorrentManager::GetTorrentInfo(const std::string& path)
	{
		boost::system::error_code ec;
		try
		{
			libtorrent::torrent_info result (path, ec);
			return result;
		}
		catch (const libtorrent::libtorrent_exception& e)
		{
			return libtorrent::torrent_info (libtorrent::sha1_hash ());
		}
	}

	void TorrentManager::Init ()
	{
		try
		{
			const std::string peerID = "IFVGrabber";
			Session_ = new libtorrent::session (libtorrent::fingerprint (peerID.c_str (), 0, 0, 1, 0),
					{6681, 6689});

			Session_->set_alert_mask (libtorrent::alert::status_notification |
					libtorrent::alert::progress_notification );
		}
		catch (const std::exception& e)
		{
			std::cout << typeid (e).name ()
					<< e.what () << std::endl;
		}
	}

	bool TorrentManager::IsTorrentFile(const std::string& path) const
	{
		boost::system::error_code ec;
		if (!boost::filesystem::exists (boost::filesystem::path (path)))
			return false;

		try
		{
			libtorrent::torrent_info (path, ec);
		}
		catch (...)
		{
			return false;
		}
		return true;
	}

	struct SimpleDispatcher
	{
		void operator() (const libtorrent::state_changed_alert& a) const
		{
			std::cout << a.message () << " 0 " << a.state << std::endl;
		}

		void operator() (const libtorrent::piece_finished_alert& a) const
		{
			std::cout << a.message () << " 1 " << a.piece_index << std::endl;
		}
	};

	void TorrentManager::QueryLibtorrentForWarnings (const boost::system::error_code& error)
	{
		if (error)
			return;

		std::auto_ptr<libtorrent::alert> a (Session_->pop_alert ());
		SimpleDispatcher sd;

		while (a.get ())
		{
			try
			{
				libtorrent::handle_alert<libtorrent::piece_finished_alert
						, libtorrent::state_changed_alert> alertHandler (a, sd);
			}
			catch (const libtorrent::libtorrent_exception& e)
			{
				std::cout << e.error ().message () << std::endl;
			}
			catch (const std::exception& e)
			{
			}

			a = Session_->pop_alert ();
		}

		Timer_->expires_at (Timer_->expires_at () + boost::posix_time::seconds (2));
		Timer_->async_wait (boost::bind (&TorrentManager::QueryLibtorrentForWarnings, this, _1));
	}
}

