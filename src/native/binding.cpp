#include <Windows.h>
#include <napi.h>
#include <atlimage.h>
#include <thread>
#include <chrono>
#include "ScreenShot.h"
#include "FindProcess.h"

using namespace ScreenShot;
using namespace Napi;
using namespace std;

Object Capture(const CallbackInfo& info) {
	string windowTitle = info[0].IsUndefined() ? "" : info[0].As<String>().Utf8Value();

	if (windowTitle == "") {
		throw Error::New(info.Env(), "No process name given.");
	}

	HWND windowHWND = FindProcessMainWindowByName(windowTitle.c_str());

	if (windowHWND == NULL) {
		string exceptionMessage = "Could not find window for process name: ";
		exceptionMessage += windowTitle;
		throw Error::New(info.Env(), exceptionMessage);
	}

	Object options = info.Length() > 1 ? info[1].As<Object>() : Object::New(info.Env());
	BOOLEAN grayscale = options.Get("grayscale").IsUndefined() ? false : options.Get("grayscale").As<Boolean>() == true;
	BOOLEAN bringToFront = options.Get("bringToFront").IsUndefined() ? true : options.Get("bringToFront").As<Boolean>() == true;
	ULONG quality = options.Get("quality").IsUndefined() ? 100 : options.Get("quality").As<Number>().Uint32Value();

	if (bringToFront) {
		SetForegroundWindow(windowHWND);

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	RECT targetWindowBounds = GetWindowBounds(windowHWND);
	CHAR* buffer;
	const DWORD bufferLen = CaptureAsBuffer(buffer, targetWindowBounds, grayscale, quality);

	if (bufferLen < 1) {
		throw Error::New(info.Env(), "Buffer storage failed.");
	}
	Object returnValue = Object::New(info.Env());
	returnValue.Set("data", Buffer<CHAR>::New(info.Env(), buffer, bufferLen));
	returnValue.Set("width", Number::New(info.Env(), targetWindowBounds.right - targetWindowBounds.left));
	returnValue.Set("height", Number::New(info.Env(), targetWindowBounds.bottom - targetWindowBounds.top));

	return returnValue;
}

Object Init(Env env, Object exports) {
	exports.Set("capture", Function::New(env, Capture));

	return exports;
}

NODE_API_MODULE(addon, Init)