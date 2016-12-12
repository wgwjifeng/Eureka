/*
 @ 0xCCCCCCCC
*/

#include <iostream>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/threading/thread.h"

#include "downloader/url_downloader.h"

class DownloadCompleteEvent : public bililive::URLDownloader::CompleteCallback {
public:
    void OnDownloadSuccess() override
    {
        std::cout << "on download success\n";
    }

    void OnDownloadStopped() override
    {
        std::cout << "on download stopped\n";
    }

    void OnDownloadFailure() override
    {
        std::cout << "on download failure\n";
    }
};

GURL url("http://photo.fanfou.com/v1/mss_3d027b52ec5a4d589e68050845611e68/ff/n0/0d/ab/yr_164422.jpg@596w_1l.jpg");
base::FilePath path(L"D:\\misc\\test.jpg");
DownloadCompleteEvent complete_event;
std::unique_ptr<bililive::URLDownloader> downloader;

int main(int argc, char* argv[])
{
    CommandLine::Init(0, nullptr);
    base::AtExitManager exit_manager;

    base::Thread worker_thread("worker");
    base::Thread::Options thread_options(base::MessageLoop::TYPE_IO, 0);
    DCHECK(worker_thread.StartWithOptions(thread_options));

    using thread_task_fn = void(*)();

    // Seems URLRequestContext requires construction and starting request must be on the same thread.
    thread_task_fn run_download = []() {
        downloader = std::make_unique<bililive::URLDownloader>(url, path, &complete_event);
        downloader->Start();
    };

    worker_thread.message_loop_proxy()->PostTask(FROM_HERE, base::Bind(run_download));

    std::cin.get();
    worker_thread.Stop();

    return 0;
}