#ifndef AWESOMIUMUIWINDOW_HPP
#define AWESOMIUMUIWINDOW_HPP

#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/BitmapSurface.h>
#include <boost/thread.hpp>
#include <locale>
#include <string>
#include <vector>

using namespace Awesomium;

class AwesomiumUiWindow : public WebViewListener::View
{

public:
	AwesomiumUiWindow(unsigned int width, unsigned int height, std::string windowTitle);
	static AwesomiumUiWindow* getAwesomiumUiWindowFromOsWindowHandle(HWND osWindowHandle);
	WebView* getMainWebView();
	void saveJpegScreenshot(std::string filename);
	void threadStart();
	void threadJoin();

	// AwesomiumUiWindow::WebViewListener::View interface implementation functions
	virtual void OnChangeTitle(WebView* caller, const WebString& title);
	virtual void OnChangeAddressBar(WebView* caller, const WebURL& url);
	virtual void OnChangeTooltip(WebView* caller, const WebString& tooltip);
	virtual void OnChangeTargetURL(WebView* caller, const WebURL& url);
	virtual void OnChangeCursor(WebView* caller, Cursor cursor);
	virtual void OnChangeFocus(WebView* caller, FocusedElementType focused_type);
	virtual void OnShowCreatedWebView(
		WebView* caller,
		WebView* new_view,
		const WebURL& opener_url,
		const WebURL& target_url,
		const Rect& initial_pos,
		bool is_popup
		);
	virtual void OnAddConsoleMessage(
		WebView* caller,
		const WebString& message,
		int line_number,
		const WebString& source
		);

private:
	void initWindow();
	void threadLoop(unsigned int n);
	HWND getOsWindowHandle();

	unsigned int width;
	unsigned int height;
	std::string windowTitle;
	WebCore* awesomiumWebCore;
	WebView* mainWebView;
	HWND osWindowHandle;
	boost::thread windowThread;
};

#endif