#include "AwesomiumUiWindow.hpp"

static std::vector<AwesomiumUiWindow*> activeAwesomiumWindows;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

AwesomiumUiWindow::AwesomiumUiWindow(unsigned int width, unsigned int height, std::string windowTitle) {
	this->width = width;
	this->height = height;
	this->windowTitle = windowTitle;
}

void AwesomiumUiWindow::initWindow() {

	// define OS window class and set attributes
	WNDCLASSEX osWindowClass;
	const wchar_t windowClassName[] = L"AwesomiumUiWindowClass";
	std::wstring wStringWindowTitle(windowTitle.begin(), windowTitle.end());
	const wchar_t* osWindowTitle = wStringWindowTitle.c_str();
	osWindowClass.cbSize = sizeof(WNDCLASSEX);
	osWindowClass.style = 0;
	osWindowClass.lpfnWndProc = WndProc;
	osWindowClass.cbClsExtra = 0;
	osWindowClass.cbWndExtra = 0;
	osWindowClass.hInstance = GetModuleHandle(0);
	osWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	osWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	osWindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	osWindowClass.lpszMenuName = NULL;
	osWindowClass.lpszClassName = windowClassName;
	osWindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register OS window class
	if (!RegisterClassEx(&osWindowClass))
		throw std::string("Failed to register OS window class");

	// create OS window
	HINSTANCE moduleHandle = GetModuleHandle(0);
	osWindowHandle = CreateWindow(
		windowClassName,
		osWindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width + 20,
		height + 40,
		NULL,
		NULL,
		moduleHandle,
		NULL
		);
	if (!osWindowHandle)
		throw std::string("Failed to create OS window");

	// init awesomium
	awesomiumWebCore = WebCore::Initialize(WebConfig());
	mainWebView = awesomiumWebCore->CreateWebView(width, height, 0, Awesomium::kWebViewType_Window);
	mainWebView->set_view_listener(this);

	// attach the Awesomium web view to the OS window
	mainWebView->set_parent_window(osWindowHandle);

	// add this Awesomium window to a global collection
	activeAwesomiumWindows.push_back(this);

	// show the window
	ShowWindow(osWindowHandle, SW_SHOWNORMAL);
	UpdateWindow(osWindowHandle);
	SetTimer(osWindowHandle, 0, 15, NULL);
}

AwesomiumUiWindow* AwesomiumUiWindow::getAwesomiumUiWindowFromOsWindowHandle(HWND osWindowHandle) {
	for (std::vector<AwesomiumUiWindow*>::iterator i = activeAwesomiumWindows.begin(); i != activeAwesomiumWindows.end(); i++) {
		if ((*i)->getOsWindowHandle() == osWindowHandle) {
			return *i;
		}
	}

	return NULL;
}

WebView* AwesomiumUiWindow::getMainWebView() {
	return mainWebView;
}

void AwesomiumUiWindow::saveJpegScreenshot(std::string filename) {
	BitmapSurface* surface = (BitmapSurface*)mainWebView->surface();
	surface->SaveToJPEG(WSLit(filename.c_str()));
}

void AwesomiumUiWindow::threadStart() {
	windowThread = boost::thread(&AwesomiumUiWindow::threadLoop, this, 3);
}

void AwesomiumUiWindow::threadJoin() {
	windowThread.join();
}

void AwesomiumUiWindow::threadLoop(unsigned int n) {
	initWindow();

	// debug - just for testing
	WebURL url(WSLit("http://www.google.com"));
	mainWebView->LoadURL(url);

	// process window messages until closure
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		awesomiumWebCore->Update();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	mainWebView->Destroy();
	WebCore::Shutdown();
}

// AwesomiumUiWindow::WebViewListener::View interface implementation functions
void AwesomiumUiWindow::OnChangeTitle(WebView* caller, const WebString& title) {}
void AwesomiumUiWindow::OnChangeAddressBar(WebView* caller, const WebURL& url) { }
void AwesomiumUiWindow::OnChangeTooltip(WebView* caller, const WebString& tooltip) { }
void AwesomiumUiWindow::OnChangeTargetURL(WebView* caller, const WebURL& url) { }
void AwesomiumUiWindow::OnChangeCursor(WebView* caller, Cursor cursor) { }
void AwesomiumUiWindow::OnChangeFocus(WebView* caller, FocusedElementType focused_type) { }
void AwesomiumUiWindow::OnShowCreatedWebView(
	WebView* caller,
	WebView* new_view,
	const WebURL& opener_url,
	const WebURL& target_url,
	const Rect& initial_pos,
	bool is_popup
	) { }
void AwesomiumUiWindow::OnAddConsoleMessage(
	WebView* caller,
	const WebString& message,
	int line_number,
	const WebString& source
	) { }

HWND AwesomiumUiWindow::getOsWindowHandle() {
	return osWindowHandle;
}

// OS window event callback
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	AwesomiumUiWindow* callingWindow = AwesomiumUiWindow::getAwesomiumUiWindowFromOsWindowHandle(hWnd);

	switch (message) {
	case WM_COMMAND:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_TIMER:
		break;
	case WM_SIZE:
		callingWindow->getMainWebView()->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_QUIT:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}