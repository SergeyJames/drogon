#include <drogon/drogon.h>
#include <chrono>

using namespace drogon;
using namespace std::chrono_literals;

int main() {

	app().registerHandler(
		"/",
		[](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
				const bool isLoggedIn = req->session()->getOptional<bool>("loggedIn").value_or(false);
				HttpResponsePtr resp;
				if (isLoggedIn == false) {
					resp = HttpResponse::newHttpViewResponse("LoginPage");
				}
				else {
					resp = HttpResponse::newHttpViewResponse("LogoutPage");
				}
				callback(resp);
		});

	app().registerHandler(
		"/logout",
		[](const HttpRequestPtr &req,
		   std::function<void(const HttpResponsePtr &)> &&callback) {
			HttpResponsePtr resp = HttpResponse::newHttpResponse();
			req->session()->erase("loggedIn");
			resp->setBody("<script>window.location.href = \"/\";</script>");
			callback(resp);
		},
		{Post});

	app().registerHandler(
		"/login",
		[](const HttpRequestPtr &req,
		   std::function<void(const HttpResponsePtr &)> &&callback) {
			HttpResponsePtr resp = HttpResponse::newHttpResponse();
			const std::string user = req->getParameter("email");
			const std::string passwd = req->getParameter("passwd");

			// NOTE: Do not use MD5 for the password hash under any
			// circumstances. We only use it because Drogon is not a
			// cryptography library, so it does not include a better hash
			// algorithm. Use Argon2 or BCrypt in a real product. username:
			// user, password: password123
			if (user == "cd57@mail.ru" && utils::getMd5(passwd) == "202CB962AC59075B964B07152D234B70") {
				req->session()->insert("loggedIn", true);
				resp->setBody("<script>window.location.href = \"/\";</script>");
				callback(resp);
			}
			else {
				resp->setStatusCode(k401Unauthorized);
				resp->setBody("<script>window.location.href = \"/\";</script>");
				callback(resp);
			}
		},
		{Post});

	const std::string host = "37.1.219.225";

	LOG_INFO << "Server running on " << host << ":8848";
	app()
		// All sessions are stored for 24 Hours
		.enableSession(24h)
		.addListener(host, 8848)
		.run();
}