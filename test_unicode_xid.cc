#include <iostream>
#include <string>

#include "unicode-xid.hh"

inline uint32_t utf8_len(const uint8_t c) {
  if (c <= 127) {
    // ascii
    return 1;
  } else if ((c & 0xE0) == 0xC0) {
    return 2;
  } else if ((c & 0xF0) == 0xE0) {
    return 3;
  } else if ((c & 0xF8) == 0xF0) {
    return 4;
  }

  // invalid
  return 0;
}

inline uint32_t to_codepoint(const char *s, int &len) {
  if (!s) {
    return ~0u;
  }

  uint32_t char_len = utf8_len(*s);

  uint32_t code = 0;
  if (char_len == 1) {
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    if (s0 > 0x7f) {
      len = 0;
      return ~0u;
    }
    code = uint32_t(s0) & 0x7f;
  } else if (char_len == 2) {
    // 11bit: 110y-yyyx 10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);

    if (((s0 & 0xe0) == 0xc0) && ((s1 & 0xc0) == 0x80)) {
      code = (uint32_t(s0 & 0x1f) << 6) | (s1 & 0x3f);
    } else {
      len = 0;
      return ~0u;
    }
  } else if (char_len == 3) {
    // 16bit: 1110-yyyy 10yx-xxxx 10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);
    unsigned char s2 = static_cast<unsigned char>(s[2]);
    if (((s0 & 0xf0) == 0xe0) && ((s1 & 0xc0) == 0x80) &&
        ((s2 & 0xc0) == 0x80)) {
      code =
          (uint32_t(s0 & 0xf) << 12) | (uint32_t(s1 & 0x3f) << 6) | (s2 & 0x3f);
    } else {
      len = 0;
      return ~0u;
    }
  } else if (char_len == 4) {
    // 21bit: 1111-0yyy 10yy-xxxx 10xx-xxxx 10xx-xxxx
    unsigned char s0 = static_cast<unsigned char>(s[0]);
    unsigned char s1 = static_cast<unsigned char>(s[1]);
    unsigned char s2 = static_cast<unsigned char>(s[2]);
    unsigned char s3 = static_cast<unsigned char>(s[3]);
    if (((s0 & 0xf8) == 0xf0) && ((s1 & 0xc0) == 0x80) &&
        ((s2 & 0xc0) == 0x80) && ((s2 & 0xc0) == 0x80)) {
      code = (uint32_t(s0 & 0x7) << 18) | (uint32_t(s1 & 0x3f) << 12) |
             (uint32_t(s2 & 0x3f) << 6) | uint32_t(s3 & 0x3f);
    } else {
      len = 0;
      return ~0u;
    }
  } else {
    len = 0;
    return ~0u;
  }

  len = char_len;
  return code;
}

///
/// Convert UTF-8 codepoint to UTF-8 string.
///
inline std::string codepoint_to_utf8(uint32_t code) {
  if (code <= 0x7f) {
    return std::string(1, char(code));
  } else if (code <= 0x7ff) {
    // 11bit: 110y-yyyx 10xx-xxxx
    uint8_t buf[2];
    buf[0] = uint8_t(((code >> 6) & 0x1f) | 0xc0);
    buf[1] = uint8_t(((code >> 0) & 0x3f) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 2);
  } else if (code <= 0xffff) {
    // 16bit: 1110-yyyy 10yx-xxxx 10xx-xxxx
    uint8_t buf[3];
    buf[0] = uint8_t(((code >> 12) & 0x0f) | 0xe0);
    buf[1] = uint8_t(((code >> 6) & 0x3f) | 0x80);
    buf[2] = uint8_t(((code >> 0) & 0x3f) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 3);
  } else if (code <= 0x10ffff) {
    // 21bit: 1111-0yyy 10yy-xxxx 10xx-xxxx 10xx-xxxx
    uint8_t buf[4];
    buf[0] = uint8_t(((code >> 18) & 0x07) | 0xF0);
    buf[1] = uint8_t(((code >> 12) & 0x3F) | 0x80);
    buf[2] = uint8_t(((code >> 6) & 0x3F) | 0x80);
    buf[3] = uint8_t(((code >> 0) & 0x3F) | 0x80);
    return std::string(reinterpret_cast<const char *>(&buf[0]), 4);
  }

  // invalid
  return std::string();
}

int main(int argc, char **argv) {
  const uint32_t max_codepoint = 0x10ffff;

  std::vector<uint32_t> codepoints;

  // NOTE: Kaomoji is not allowed for Default Identifier.
  std::string input = u8"0/ª😩"; // ª = 0xaa

  if (argc > 1) {
    // Assume stdin console is UTF-8 encoding.
    input = argv[1];
  }

  for (size_t i = 0; i < input.size();) {
    int char_len;
    uint32_t cp = to_codepoint(input.c_str() + i, char_len);
    if ((cp >= max_codepoint) || (char_len == 0)) {
      std::cerr << "Invalid UTF-8 string\n";
    }
    codepoints.push_back(cp);

    i += uint32_t(char_len);
  }

  for (size_t i = 0; i < codepoints.size(); i++) {
    std::cout << "cp " << codepoints[i] << "(0x" << std::hex << codepoints[i] << ", " << codepoint_to_utf8(codepoints[i]) << ") XID_Start "
              << (unicode_xid::is_xid_start(codepoints[i]) ? "true" : "false")
              << ", XID_Continue "
              << (unicode_xid::is_xid_continue(codepoints[i]) ? "true"
                                                              : "false")
              << "\n";
  }

  return 0;
}
