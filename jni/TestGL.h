#ifndef TESTGL_H_
#define TESTGL_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "TimeDuration.h"
#include <android/log.h>
#include <stdio.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "gl-measure", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "gl-measure", __VA_ARGS__))

#define CHECK_GLERROR() check_gl_error(__FILE__, __LINE__)
static inline void check_gl_error(const char *file, int line)
{
        glFinish();
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
           LOGW("GL error(%x): %s:%d",error, file, line);
        }
}

// A single GL test to run. Handles glFinish-ing before and after the
// test. The idea is that this test will be run multipe times by
// TestGLRunner. Subclass TestGL to implement specific tests.
class TestGL {
public:
  enum PixelSize {PixelSize16Bits, PixelSize24Bits, PixelSize32Bits};

  TestGL(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment);
  void RunTest();
  virtual ~TestGL();
  virtual void TestAndOutput(int aNumIterations, FILE* aFile);

  unsigned int GetPixelSize() {return mPixelSize;}

protected:
  virtual void PreTest()
  {
    glFinish();
  }

  virtual void Test() {}

  virtual void PostTest()
  {
    glFinish();
    mTimesTested++;
  }

  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "TestGL");
  }

  GLuint mFormat;
  GLuint mType;
  GLsizei mTextureSize;
  GLint mAlignment;
  GLuint* mTextureID;
  static const unsigned int mNumTextures = 1;
  unsigned int mPixelSize;
  unsigned char* mBuffer;
  unsigned int mTimesTested;
};

class TestGLRunner {
public:
  TestGLRunner(TestGL* aTest) : mTest(aTest) {}

  Duration Run(int aNumIterations);
protected:
  TestGL *mTest;
};

class TestTexImage2D : public TestGL {
public:
  TestTexImage2D(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment);

protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "TexImage2D");
  }
};

class TestTexSubImage2D : public TestTexImage2D {
public:
  TestTexSubImage2D(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                    GLsizei aUploadWidth, GLint aOffset);
  virtual void TestAndOutput(int aNumIterations, FILE* aFile);
protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "TexSubImage2D");
  }

 GLsizei mUploadWidth;
 GLint mOffset;
};

class TestTexSubImage2DMemcpy : public TestTexSubImage2D {
public:
  TestTexSubImage2DMemcpy(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                          GLsizei aUploadWidth, GLint aOffset);

protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "TexSubImage2DMemcpy");
  }
};

class TestTexSubImage2DRowByRow : public TestTexSubImage2D {
public:
  TestTexSubImage2DRowByRow(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                            GLsizei aUploadWidth, GLint aOffset);

protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "TexSubImage2DRowByRow");
  }
};

class TestTexSubImage2DTopHalf : public TestTexSubImage2D {
public:
  TestTexSubImage2DTopHalf(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                           GLsizei aUploadWidth, GLint aOffset);


protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "TexSubImage2DTopHalf");
  }
};

class TestUnpackRowLength : public TestGL {
public:
  TestUnpackRowLength(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                      GLsizei aUploadWidth, GLint aOffset);
  virtual void TestAndOutput(int aNumIterations, FILE* aFile);

protected:
  virtual void Test();
  virtual void OutputTestName(FILE* aFile)
  {
    fprintf(aFile, "%20s", "UnpackRowLength");
  }

  GLsizei mUploadWidth;
  GLint mOffset;
};
#endif /* TESTGL_H_ */
