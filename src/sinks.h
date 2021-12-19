#pragma once

#include "filters.h"
#include "formaters.h"
#include <memory>
#include <vector>

namespace asynclog
{

using  FilteredSinkPtr=std::shared_ptr<IFilteredSink>;
using  SinkPtr = std::shared_ptr<ISink>;
using  FilterPtr = std::shared_ptr<IFilter>;
using  FormatterPtr = std::shared_ptr<IFormatter>;


// composite pattern, make possible to use many sinks
class SinkComposite : public IFilteredSink
{
    std::vector<FilteredSinkPtr> sinks;

public:
    void Log(const Logdata& logdata) override;

    bool Enabled(const Logdata& logdata) override;

    void SetReportingLevel(LogLevel level) override;

    void AddSink(const FilteredSinkPtr& os);

    void Clear();
};

class FilteredSink : public IFilteredSink
{
protected:
    SinkPtr sink;
    FilterPtr filter;

public:
    FilteredSink(SinkPtr asink, FilterPtr afilter = FilterPtr(new AreaFilter()));

    void Log(const Logdata& logdata) override;

    bool Enabled(const Logdata& logdata) override;

    void SetReportingLevel(LogLevel level) override;
};


}
