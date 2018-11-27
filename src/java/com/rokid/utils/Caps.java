package com.rokid.utils;

import java.io.InputStream;
import java.io.IOException;

public class Caps {
  public static Caps create() {
    long h = nativeCreate();
    return new Caps(h);
  }

  public static Caps parse(byte[] data, int offset, int length) throws CapsException {
    int arrLen = data == null ? 0 : data.length;
    checkOffsetAndCount(arrLen, offset, length);
    long h = nativeParse(data, offset, length);
    if (h == 0)
      throw new CapsException(CapsException.CODE_CORRUPTED_DATA);
    return new Caps(h);
  }

  public static Caps parse(byte[] data) throws CapsException {
    if (data == null)
      throw new CapsException(CapsException.CODE_INVALID_PARAM);
    return parse(data, 0, data.length);
  }

  private Caps() {
  }

  private Caps(long h) {
    _handle = h;
  }

  public void write(int v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(float v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(long v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(double v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(String v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(byte[] data, int offset, int length) throws CapsException {
    int arrLen = data == null ? 0 : data.length;
    checkOffsetAndCount(arrLen, offset, length);
    int r = nativeWrite(data, offset, length);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(byte[] data) throws CapsException {
    int arrLen = data == null ? 0 : data.length;
    int r = nativeWrite(data, 0, arrLen);
    if (r != 0)
      throw new CapsException(r);
  }

  public void write(Caps v) throws CapsException {
    int r = nativeWrite(v);
    if (r != 0)
      throw new CapsException(r);
  }

  public int readInt() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_INTEGER);
    if (r.err != 0)
      throw new CapsException(r.err);
    return (int)r.integer;
  }

  public float readFloat() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_FLOAT);
    if (r.err != 0)
      throw new CapsException(r.err);
    return (float)r.number;
  }

  public long readLong() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_LONG);
    if (r.err != 0)
      throw new CapsException(r.err);
    return r.integer;
  }

  public double readDouble() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_DOUBLE);
    if (r.err != 0)
      throw new CapsException(r.err);
    return r.number;
  }

  public String readString() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_STRING);
    if (r.err != 0)
      throw new CapsException(r.err);
    return (String)r.value;
  }

  public InputStream readBinary() throws CapsException {
    ReadBinaryResult r = nativeReadBinary();
    if (r.err != 0)
      throw new CapsException(r.err);
    return new NativeInputStream(r.byteArray, r.size);
  }

  public Caps readCaps() throws CapsException {
    ReadResult r = nativeRead(MEMBER_TYPE_CAPS);
    if (r.err != 0)
      throw new CapsException(r.err);
    return new Caps(r.integer);
  }

  public int serialize(byte[] result, int offset, int length, int flags) throws CapsException {
    int arrLen = result == null ? 0 : result.length;
    checkOffsetAndCount(arrLen, offset, length);
    int r = nativeSerialize(result, offset, length, flags);
    if (r > length)
      throw new CapsException(CapsException.CODE_INSUFF_BUFFER, r);
    return r;
  }

  public int serialize(byte[] result, int flags) throws CapsException {
    int arrLen = result == null ? 0 : result.length;
    return serialize(result, 0, arrLen, flags);
  }

  public byte[] serialize(int flags) throws CapsException {
    int r = nativeSerialize(null, 0, 0, flags);
    byte[] data = new byte[r];
    nativeSerialize(data, 0, data.length, flags);
    return data;
  }

  public void destroy() {
    if (_handle != 0 && refCount == 0) {
      nativeDestroy();
      _handle = 0;
    }
  }

  public void finalize() {
    destroy();
  }

  private void refUp() {
    ++refCount;
  }

  private void refDown() {
    if (refCount <= 0)
      return;
    --refCount;
    if (refCount == 0)
      destroy();
  }

  private static void checkOffsetAndCount(int arrayLength, int offset, int count) {
    if ((offset | count) < 0 || offset > arrayLength || arrayLength - offset < count) {
      throw new ArrayIndexOutOfBoundsException("arrayLength: " + arrayLength + ", offset: " + offset + ", count: " + count);
    }
  }


  private class NativeInputStream extends InputStream {
    private NativeInputStream(long nba, int size) {
      // prevent native Caps free before NativeInputStream close
      refUp();
      nativeByteArray = nba;
      totalSize = size;
    }

    public int read() throws IOException {
      if (closed)
        throw new IOException();
      if (curReadBytes < totalSize)
        return nativeGetByIndex(nativeByteArray, curReadBytes++);
      return -1;
    }

    public int read(byte[] result, int offset, int length) throws IOException {
      if (closed)
        throw new IOException();
      int arrLen = result == null ? 0 : result.length;
      checkOffsetAndCount(arrLen, offset, length);
      int remain = totalSize - curReadBytes;
      int readBytes;
      if (remain == 0)
        return -1;
      if (length >= remain)
        readBytes = remain;
      else
        readBytes = length;
      nativeGet(nativeByteArray, curReadBytes, result, offset, readBytes);
      curReadBytes += readBytes;
      return readBytes;
    }

    public void close() {
      if (!closed) {
        refDown();
        closed = true;
      }
    }

    public void finalize() {
      close();
    }

    private native int nativeGetByIndex(long nba, int index);

    private native void nativeGet(long nba, int from, byte[] result, int offset, int length);

    private long nativeByteArray;
    private int totalSize;
    private int curReadBytes;
    private boolean closed;
  }

  private static class ReadResult {
    public int err;
    public long integer;
    public double number;
    Object value;
  }

  private static class ReadBinaryResult {
    public int err;
    public long byteArray;
    public int size;
  }

  private static native long nativeCreate();

  private static native long nativeParse(byte[] data, int offset, int length);

  private native ReadResult nativeRead(int type);

  private native ReadBinaryResult nativeReadBinary();

  private native int nativeWrite(int v);

  private native int nativeWrite(float v);

  private native int nativeWrite(long v);

  private native int nativeWrite(double v);

  private native int nativeWrite(String v);

  private native int nativeWrite(byte[] data, int offset, int length);

  private native int nativeWrite(Caps v);

  private native int nativeSerialize(byte[] result, int offset, int length, int flags);

  private native void nativeDestroy();

  private static native void nativeInit(Class<ReadResult> cls1, Class<ReadBinaryResult> cls2);

  private long _handle;

  private int refCount;

  private static final int MEMBER_TYPE_INTEGER = 0;
  private static final int MEMBER_TYPE_FLOAT = 1;
  private static final int MEMBER_TYPE_LONG = 2;
  private static final int MEMBER_TYPE_DOUBLE = 3;
  private static final int MEMBER_TYPE_STRING = 4;
  private static final int MEMBER_TYPE_CAPS = 5;
  public static final int FLAG_NET_BYTEORDER = 0x80;

  static {
    System.loadLibrary("caps-jni");
    nativeInit(ReadResult.class, ReadBinaryResult.class);
  }
}
