# packaging of aws-cpp-sdk library

Build of debian packages for Buster.

## Package dependencies
- libcurl4-dev
  Available in these versions:
  - libcurl4-gnutls-dev - development files and documentation for libcurl (GnuTLS flavour)
    depends extra on: libgnutls30, libnettle6
  - libcurl4-nss-dev - development files and documentation for libcurl (NSS flavour)
    depends extra on: libnspr4, libnss3
  - libcurl4-openssl-dev - development files and documentation for libcurl (OpenSSL flavour)
    depends extra on: libssl1.1


## SDK libs dependencies
- aws-cpp-sdk-core (libaws-cpp-sdk-core.so)
  - aws-cpp-sdk-s3 (libaws-cpp-sdk-s3.so)
    - aws-cpp-sdk-transfer (libaws-cpp-sdk-transfer.so)


## Third party dependencies:
 - aws-c-common
 - aws-checksums
 - aws-c-event-stream

Dependencies are by default build and install into system at the begining of aws-cpp-sdk library build.
This was modifiled to install third party dependencies intu build tree and artefacts are included in
`aws-cpp-sdk-core-deb` and `aws-cpp-sdk-core0` packages.
