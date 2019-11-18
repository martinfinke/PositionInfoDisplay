/*
 Author: Martin Finke
 Date:   Nov 18, 2019 08:54
 */

#include "positioninfodisplay.h"

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

#include "third_party/readerwriterqueue.h"

#include <utility>

namespace positioninfodisplay {
using namespace juce;

using std::move;
using std::pair;

template<typename T, size_t MAX_BLOCK_SIZE = 512>
using ReaderWriterQueue = moodycamel::ReaderWriterQueue<T, MAX_BLOCK_SIZE>;

#include "positioninfodisplay/Processor.h"
#include "positioninfodisplay/Editor.h"
#include "positioninfodisplay/Processor.cpp"
#include "positioninfodisplay/Editor.cpp"
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new positioninfodisplay::Processor();
}
