#pragma once
#include "../compression_interface.h"

#include "deflate_properties.h"

#include "../../extlibs/zlib/zlib.h"

#include <cstdint>

template <typename ELEM_TYPE, typename TRAITS_TYPE>
class basic_deflate_encoder
  : public compression_encoder_interface_basic<ELEM_TYPE, TRAITS_TYPE>
{
  public:
    typedef typename compression_interface_basic<ELEM_TYPE, TRAITS_TYPE>::istream_type istream_type;
    typedef typename compression_interface_basic<ELEM_TYPE, TRAITS_TYPE>::ostream_type ostream_type;

    basic_deflate_encoder()
    {

    }

    ~basic_deflate_encoder()
    {
      if (is_init())
      {
        deflateEnd(&_zstream);
        uninit_buffers();
      }
    }

    void init(ostream_type& stream) override
    {
      deflate_properties props;
      init(stream, props);
    }

    void init(ostream_type& stream, compression_properties_interface& props) override
    {
      // init stream
      _stream = &stream;

      // init buffers
      deflate_properties& deflateProps = static_cast<deflate_properties&>(props);
      _bufferCapacity = deflateProps.BufferCapacity;

      uninit_buffers();
      _inputBuffer = new ELEM_TYPE[_bufferCapacity];
      _outputBuffer = new ELEM_TYPE[_bufferCapacity];

      // init deflate
      _zstream.zalloc = nullptr;
      _zstream.zfree = nullptr;
      _zstream.opaque = nullptr;

      _zstream.next_in = nullptr;
      _zstream.next_out = nullptr;
      _zstream.avail_in = 0;
      _zstream.avail_out = 0;

      deflateInit2(&_zstream, deflateProps.CompressionLevel, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    }

    bool is_init() const override
    {
      return _stream != nullptr;
    }

    ELEM_TYPE* get_buffer_begin() override
    {
      return _inputBuffer;
    }

    ELEM_TYPE* get_buffer_end() override
    {
      return _inputBuffer + _bufferCapacity;
    }

    void encode_next(size_t length) override
    {
      // set the input buffer
      _zstream.next_in = reinterpret_cast<Bytef*>(_inputBuffer);
      _zstream.avail_in = static_cast<uInt>(length);

      bool flush = length < _bufferCapacity;

      // compress data
      do {
        // zstream output
        _zstream.next_out = reinterpret_cast<Bytef*>(_outputBuffer);
        _zstream.avail_out = static_cast<uInt>(_bufferCapacity);

        // compress stream
        deflate(&_zstream, flush ? Z_FINISH : Z_NO_FLUSH);

        size_t have = _bufferCapacity - static_cast<size_t>(_zstream.avail_out);

        if (have > 0)
        {
          _stream->write(_outputBuffer, have);
        }
      } while (_zstream.avail_out == 0);
    }

    void sync() override
    {

    }

  private:
    void uninit_buffers()
    {
      delete[] _inputBuffer;
      delete[] _outputBuffer;
    }

    bool zlib_suceeded(int errorCode)
    {
      return ((_lastError = errorCode) >= 0);
    }

    z_stream    _zstream;                   // internal zlib structure
    int         _lastError      = Z_OK;     // last error of zlib operation

    ostream_type* _stream       = nullptr;

    size_t     _bufferCapacity  = 0;
    ELEM_TYPE* _inputBuffer     = nullptr;  // pointer to the start of the input buffer
    ELEM_TYPE* _outputBuffer    = nullptr;  // pointer to the start of the output buffer
};

typedef basic_deflate_encoder<uint8_t, std::char_traits<uint8_t>>  byte_deflate_encoder;
typedef basic_deflate_encoder<char, std::char_traits<char>>        deflate_encoder;
typedef basic_deflate_encoder<wchar_t, std::char_traits<wchar_t>>  wdeflate_encoder;
