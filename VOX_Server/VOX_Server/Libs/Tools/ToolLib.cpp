#include "ToolLib.h"

void ToolLib::fillPendingList(vector<string>& result, unordered_set<string>& source)
{
    result.reserve(source.size());
    result.assign(source.begin(), source.end());
    source.clear();
}
