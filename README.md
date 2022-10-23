# tuba

The skeletons of a shared library that can export logs/metrics/traces to OpenTelemetry from C++.
The goal is to end up with small .dll, and bunch of generated grpc/protobuf files and ship this as "sdk"

I did an alternative way using vcpkg/cmake, this is just proof of concept and some bzlmod learnings

## Howto?

Download bazel, or even better bazelisk (and rename it to bazel (bazel.exe))

Then from the repo

```
bazel run :something
```

Obviously this won't do much, unless you have otel running with the host:port used in something.cpp

There would be ../.out folder with cache, repo and build artifacts. Feel free to delete after.
