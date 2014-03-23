#pragma once
#include "../compression/compression_interface.h"
#include "../encryption/encryption_interface.h"
#include "ZipMethodDescriptor.h"

#include <memory>

class ZipMethodBase
{
  public:
    static const uint16_t StoredCompressionMethod = 0;
    static const uint16_t StoredVersionNeededToExtract = 10;

    virtual const ZipMethodDescriptor& GetZipMethodDescriptor() const = 0;
    static const ZipMethodDescriptor& GetZipMethodDescriptorStatic()
    {
      // Default "Stored method" descriptor.
      static ZipMethodDescriptor zmd {
        StoredCompressionMethod,
        StoredVersionNeededToExtract
      };
      return zmd;
    }

  protected:
    ZipMethodBase() { }

  private:
    ZipMethodBase(const ZipMethodBase&) = delete;
    ZipMethodBase& operator = (const ZipMethodBase&) = delete;
};

template <
  typename ENCODER_TYPE, typename DECODER_TYPE, typename PROPS_TYPE
>
class ZipMethod
  : public ZipMethodBase
{
  public:
    typedef std::shared_ptr<ENCODER_TYPE> encoder_t;
    typedef std::shared_ptr<DECODER_TYPE> decoder_t;
    typedef PROPS_TYPE                    properties_t;

    void SetEncoder(encoder_t encoder) { _encoder = encoder; }
    void SetDecoder(decoder_t decoder) { _decoder = decoder; }

    encoder_t GetEncoder() const { return _encoder; }
    decoder_t GetDecoder() const { return _decoder; }

    virtual properties_t& GetProperties() = 0;

  protected:
    ZipMethod() { }

  private:
    encoder_t _encoder;
    decoder_t _decoder;
};
