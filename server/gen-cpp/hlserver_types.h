/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef hlserver_TYPES_H
#define hlserver_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace hlserver {

struct JobType {
  enum type {
    ON_DEMAND = 1,
    LIVE_STREAM = 2,
    SCHEDULING = 3
  };
};

extern const std::map<int, const char*> _JobType_VALUES_TO_NAMES;

struct JobStatusType {
  enum type {
    INVALID = 1,
    ACCEPTED = 2,
    IN_PROGRESS = 3,
    FINISHED = 4,
    ERROR = 6
  };
};

extern const std::map<int, const char*> _JobStatusType_VALUES_TO_NAMES;

typedef int8_t i8;

class Job;

class JobStatus;

class TranscodingError;

class JobError;

typedef struct _Job__isset {
  _Job__isset() : id(false), type(false) {}
  bool id;
  bool type;
} _Job__isset;

class Job {
 public:

  static const char* ascii_fingerprint; // = "DFA40D9D2884599F3D1E7A57578F1384";
  static const uint8_t binary_fingerprint[16]; // = {0xDF,0xA4,0x0D,0x9D,0x28,0x84,0x59,0x9F,0x3D,0x1E,0x7A,0x57,0x57,0x8F,0x13,0x84};

  Job(const Job&);
  Job& operator=(const Job&);
  Job() : id(0), type((JobType::type)0) {
  }

  virtual ~Job() throw();
  int64_t id;
  JobType::type type;

  _Job__isset __isset;

  void __set_id(const int64_t val);

  void __set_type(const JobType::type val);

  bool operator == (const Job & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(type == rhs.type))
      return false;
    return true;
  }
  bool operator != (const Job &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Job & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(Job &a, Job &b);

typedef struct _JobStatus__isset {
  _JobStatus__isset() : type(false), jobId(false), message(false) {}
  bool type;
  bool jobId;
  bool message;
} _JobStatus__isset;

class JobStatus {
 public:

  static const char* ascii_fingerprint; // = "5903DB663ABC109180254BE53D624B46";
  static const uint8_t binary_fingerprint[16]; // = {0x59,0x03,0xDB,0x66,0x3A,0xBC,0x10,0x91,0x80,0x25,0x4B,0xE5,0x3D,0x62,0x4B,0x46};

  JobStatus(const JobStatus&);
  JobStatus& operator=(const JobStatus&);
  JobStatus() : type((JobStatusType::type)0), jobId(0), message() {
  }

  virtual ~JobStatus() throw();
  JobStatusType::type type;
  int64_t jobId;
  std::string message;

  _JobStatus__isset __isset;

  void __set_type(const JobStatusType::type val);

  void __set_jobId(const int64_t val);

  void __set_message(const std::string& val);

  bool operator == (const JobStatus & rhs) const
  {
    if (!(type == rhs.type))
      return false;
    if (!(jobId == rhs.jobId))
      return false;
    if (__isset.message != rhs.__isset.message)
      return false;
    else if (__isset.message && !(message == rhs.message))
      return false;
    return true;
  }
  bool operator != (const JobStatus &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const JobStatus & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(JobStatus &a, JobStatus &b);

typedef struct _TranscodingError__isset {
  _TranscodingError__isset() : code(false), what(false) {}
  bool code;
  bool what;
} _TranscodingError__isset;

class TranscodingError : public ::apache::thrift::TException {
 public:

  static const char* ascii_fingerprint; // = "3F5FC93B338687BC7235B1AB103F47B3";
  static const uint8_t binary_fingerprint[16]; // = {0x3F,0x5F,0xC9,0x3B,0x33,0x86,0x87,0xBC,0x72,0x35,0xB1,0xAB,0x10,0x3F,0x47,0xB3};

  TranscodingError(const TranscodingError&);
  TranscodingError& operator=(const TranscodingError&);
  TranscodingError() : code(0), what() {
  }

  virtual ~TranscodingError() throw();
  int32_t code;
  std::string what;

  _TranscodingError__isset __isset;

  void __set_code(const int32_t val);

  void __set_what(const std::string& val);

  bool operator == (const TranscodingError & rhs) const
  {
    if (!(code == rhs.code))
      return false;
    if (!(what == rhs.what))
      return false;
    return true;
  }
  bool operator != (const TranscodingError &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TranscodingError & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(TranscodingError &a, TranscodingError &b);

typedef struct _JobError__isset {
  _JobError__isset() : jobId(false), what(false) {}
  bool jobId;
  bool what;
} _JobError__isset;

class JobError : public ::apache::thrift::TException {
 public:

  static const char* ascii_fingerprint; // = "727CAEA8265A5DE67DBC931F55CD8753";
  static const uint8_t binary_fingerprint[16]; // = {0x72,0x7C,0xAE,0xA8,0x26,0x5A,0x5D,0xE6,0x7D,0xBC,0x93,0x1F,0x55,0xCD,0x87,0x53};

  JobError(const JobError&);
  JobError& operator=(const JobError&);
  JobError() : jobId(0), what() {
  }

  virtual ~JobError() throw();
  int64_t jobId;
  std::string what;

  _JobError__isset __isset;

  void __set_jobId(const int64_t val);

  void __set_what(const std::string& val);

  bool operator == (const JobError & rhs) const
  {
    if (!(jobId == rhs.jobId))
      return false;
    if (!(what == rhs.what))
      return false;
    return true;
  }
  bool operator != (const JobError &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const JobError & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(JobError &a, JobError &b);

} // namespace

#endif
