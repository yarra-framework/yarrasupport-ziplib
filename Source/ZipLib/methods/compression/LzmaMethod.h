#pragma once
#include "../MethodTemplate.h"
#include "../../compression/lzma/lzma_encoder.h"
#include "../../compression/lzma/lzma_decoder.h"

#include <memory>

class LzmaMethod :
  public CompressionMethodTemplate
  <
    LzmaMethod,
    lzma_encoder, lzma_decoder, lzma_properties,
    /* CompressionMethod */ 14,
    /* VersionNeededToExtract */ 63
  >
{
  public:
    enum class CompressionLevel : int
    {
      L1 = 1,
      L2 = 2,
      L3 = 3,
      L4 = 4,
      L5 = 5,
      L6 = 6,
      L7 = 7,
      L8 = 8,
      L9 = 9,

      Fastest = L1,
      Default = L6,
      Best = L9
    };

    bool GetIsMultithreaded() const { return _properties.IsMultithreaded; }
    void SetIsMultithreaded(bool isMultithreaded) { _properties.IsMultithreaded = isMultithreaded; }

    CompressionLevel GetCompressionLevel() const { return static_cast<CompressionLevel>(_properties.CompressionLevel); }
    void SetCompressionLevel(CompressionLevel compressionLevel) { _properties.CompressionLevel = static_cast<int>(compressionLevel); }
};
