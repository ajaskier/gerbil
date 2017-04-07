#include "tbb_task.h"
#include "subscription.h"
#include "lock.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

TbbTask::TbbTask(QString target, std::map<QString, SourceDeclaration> sources)
    : Task(target, sources)
{}

TbbTask::TbbTask(QString id, QString target, std::map<QString, SourceDeclaration> sources)
    : Task(id, target, sources)
{}
