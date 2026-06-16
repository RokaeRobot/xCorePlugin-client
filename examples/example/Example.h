#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "Example_global.h"
#include "plugincommon.h"

namespace  xplugin{
class EXAMPLE_EXPORT ExampleClass : public PluginBase
{
public:
    ExampleClass(QObject *parent = nullptr);

private:
    void init() override;
    void afterInit() override;

};
}

#endif // EXAMPLE_H
