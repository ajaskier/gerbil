#include "task_distviewbins_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"
#include "rectangles.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

#include "distviewcompute_utils.h"

class Accumulate2 {
public:
    Accumulate2(bool subtract, multi_img &multi, const cv::Mat1s &labels, const cv::Mat1b &mask,
        int nbins, multi_img::Value binsize, multi_img::Value minval, bool ignoreLabels,
        std::vector<multi_img::Value> &illuminant,
        std::vector<BinSet> &sets)
        : subtract(subtract), multi(multi), labels(labels), mask(mask), nbins(nbins), binsize(binsize),
        minval(minval), illuminant(illuminant), ignoreLabels(ignoreLabels), sets(sets) {}
    void operator()(const tbb::blocked_range2d<int> &r) const;
private:
    bool subtract;
    multi_img &multi;
    const cv::Mat1s &labels;
    const cv::Mat1b &mask;
    int nbins;
    multi_img::Value binsize;
    multi_img::Value minval;
    bool ignoreLabels;
    std::vector<multi_img::Value> &illuminant;
    std::vector<BinSet> &sets;

protected:

};

void Accumulate2::operator()(const tbb::blocked_range2d<int> &r) const
{
    for (int y = r.rows().begin(); y != r.rows().end(); ++y) {
        const short *lr = labels[y];
        const uchar *mr = (mask.empty() ? 0 : mask[y]);
        for (int x = r.cols().begin(); x != r.cols().end(); ++x) {
            if (mr && !mr[x])
                continue;

            int label = (ignoreLabels ? 0 : lr[x]);
            label = (label >= (int)sets.size()) ? 0 : label;
            const multi_img::Pixel& pixel = multi(y, x);
            BinSet &s = sets[label];

            BinSet::HashKey hashkey(multi.size());
            for (unsigned int d = 0; d < multi.size(); ++d) {
                int pos = floor(Compute::curpos(
                                    pixel[d], d, minval, binsize, illuminant));
                pos = std::max(pos, 0); pos = std::min(pos, nbins-1);
                hashkey[d] = (unsigned char)pos;
            }

            if (subtract) {
                BinSet::HashMap::accessor ac;
                if (s.bins.find(ac, hashkey)) {
                    ac->second.sub(pixel);
                    if (ac->second.weight == 0.f)
                        s.bins.erase(ac);
                }
                ac.release();
                s.totalweight--; // atomic
            } else {
                BinSet::HashMap::accessor ac;
                s.bins.insert(ac, hashkey);
                ac->second.add(pixel);
                ac.release();
                s.totalweight++; // atomic
            }
        }
    }
}


TaskDistviewBinsTbb::TaskDistviewBinsTbb(QString id, QString target, std::map<QString, QString> sources,
//                         const cv::Mat1s &labels,
//                         QVector<QColor> &colors,
                         std::vector<multi_img::Value> &illuminant,
                         //ViewportCtx &args,
                         const cv::Mat1b &mask)//,
                         //bool inplace, bool apply)
    : Task(id, target, sources), /*labels(labels), colors(colors),*/ illuminant(illuminant),
      mask(mask)//, inplace(inplace), apply(apply)
{

}

TaskDistviewBinsTbb::~TaskDistviewBinsTbb()
{

}

bool TaskDistviewBinsTbb::run()
{
    return true;
}

void TaskDistviewBinsTbb::expression(bool subtract, std::vector<cv::Rect> &collection,
                                 multi_img &multi, std::vector<BinSet> &sets,
                                 const ViewportCtx *args)
{

    for (auto it = collection.begin(); it != collection.end(); ++it) {
        Accumulate2 expr(subtract, multi, labels, mask, args->nbins, args->binsize,
                            args->minval, args->ignoreLabels, illuminant, sets);

        tbb::parallel_for(
                    tbb::blocked_range2d<int>(it->y, it->x + it->height,
                                              it->x, it->x + it->width),
                    expr, tbb::auto_partitioner(), stopper);
    }

}

void TaskDistviewBinsTbb::createBinSets(multi_img &multi, std::vector<BinSet> &sets)
{
    for (int i = sets.size(); i < colors.size(); ++i) {
        sets.push_back(BinSet(colors[i], multi.size()));
    }
}

void TaskDistviewBinsTbb::updateContext(multi_img &multi, ViewportCtx *args)
{
    args->dimensionality = multi.size();
    args->meta = multi.meta;
    args->xlabels.resize(multi.size());
    for (unsigned int i = 0; i < multi.size(); ++i) {
        if (!multi.meta[i].empty) {
            args->xlabels[i].setNum(multi.meta[i].center, 'g', 4);
        } else {
            //GGDBGM(i << " meta is empty. "<< args.type << endl);
        }
    }
    args->binsize = (multi.maxval - multi.minval)
                    / (multi_img::Value)(args->nbins - 1);
    args->minval = multi.minval;
    args->maxval = multi.maxval;

    args->valid = true;
}
