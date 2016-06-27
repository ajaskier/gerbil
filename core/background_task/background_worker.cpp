
#include "background_worker.h"
#include "background_executor.h"

void BackgroundWorker::run() {

    while(running) {

        std::cout << "gotta take task\n";
        BackgroundTaskPtr task = executor.pop();
        auto success = task->start();

        emit taskDone(success);
        std::cout << "done with the task!\n";
    }

    std::cout << "I'm done working!";

}
