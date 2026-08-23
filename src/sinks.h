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

class FilteredSink : public IFilteredSink
{
protected:
    SinkPtr sink;
    FilterPtr filter;

public:
    FilteredSink(SinkPtr asink, FilterPtr afilter = FilterPtr(new AreaFilter()));

    void Log(const Logdata& logdata) override;
    virtual void Log(Logdata&& logdata) override;

    bool Enabled(LogLevel level, const std::string& area) override;

    void SetReportingLevel(LogLevel level) override;

};


}
