#include "Curl.hpp"
#ifdef TEST
#include "MD5.hpp"
#endif
string LoginURL = "https://cas2.edu.sh.cn/CAS/login?service=https%3A%2F%2Fsmilelogin.shec.edu.cn%2Fsmile-login%2Fauth%2Fcas%2Flogin%3Ftarget%3Dhttps%3A%2F%2Fsmile.shec.edu.cn%2F%3Fpath%3DairClassroomPage";
int main()
{
    CLN_TRY
    cout << "Checking login... " << flush;
    int HTTPResponseCode = 0;
    GetDataToFile(LoginURL, "Header.tmp", "Body.tmp", false, "", NULL, &HTTPResponseCode);
    if (HTTPResponseCode == 200)
    {
        cout << "Not logged in" << endl
             << "Logging in... " << flush;
        GetDataToFile(LoginURL,
                      "Header.tmp",
                      "Body.tmp",
                      true,
                      "username=" + GetDataFromFileToString("Keys/CASUsername") +
                          "&password=" + GetDataFromFileToString("Keys/CASPassword") +
                          "&role=1001&openid=&execution=" +
                          GetStringBetween(GetDataFromFileToString(),
                                           "<input type=\"hidden\" id=\"execution_id\" name=\"execution\" value=\"",
                                           "\"") +
                          "&checkFlag=temp&_eventId=submit",
                      NULL,
                      &HTTPResponseCode,
                      "application/x-www-form-urlencoded");
        if (HTTPResponseCode != 302)
        {
            cout << "Failed" << endl;
            return 0;
        }
        cout << "Succeed" << endl;
    }
    else
        cout << "Already logged in" << endl;
    cout << "Redirect login... " << flush;
    string RedirectURL = FindLocation();
    GetDataToFile(RedirectURL);
    cout << "Success" << endl
         << "Getting class list size... " << flush;
    json ClassesRequest;
    ClassesRequest["start"] = 1;
    ClassesRequest["length"] = 1;
    ClassesRequest["showNotStudy"] = 1;
    ClassesRequest["sectionId"] = 2;
    ClassesRequest["smileSubjectId"] = "";
    ClassesRequest["gradeId"] = "8";
    ClassesRequest["semesterNo"] = "1";
    GetDataToFile("https://smile.shec.edu.cn/smile-index-service/api/index/registerResource/pageResource",
                  "Header.tmp",
                  "Body.tmp",
                  true,
                  ClassesRequest.dump());
    json Classes = json::parse(GetDataFromFileToString());
    ClassesRequest["length"] = atoi(Classes["data"]["total"].as_string().c_str());
    cout << "Success" << endl
         << "Getting class list... " << flush;
    GetDataToFile("https://smile.shec.edu.cn/smile-index-service/api/index/registerResource/pageResource",
                  "Header.tmp",
                  "Body.tmp",
                  true,
                  ClassesRequest.dump());
    cout << "Success" << endl
         << "Parsing data... " << flush;
    Classes = json::parse(GetDataFromFileToString());
    queue<pair<string, string>> VideoList;
    for (auto i : Classes["data"]["list"])
        VideoList.push(make_pair(i["subjectName"].as_string() + "_" +
                                     i["finalPremiereTime"].as_string() + "_" +
                                     i["unitName"].as_string() + "_" +
                                     i["name"].as_string() + ".mp4",
                                 i["encryptUrl"].as_string()));
    int Size = VideoList.size();
    cout << "Success" << endl;
#ifdef TEST
    string CheckFileName = "";
#endif
    while (!VideoList.empty())
    {
        cout << "\033cDownloading "
             << Size - VideoList.size() + 1 << "/" << Size << "  " << VideoList.front().first << endl;
        if (1)
        {
            GetDataToFile(VideoList.front().second,
                          "Header.tmp",
                          VideoList.front().first,
                          false,
                          "",
                          NULL,
                          NULL,
                          "application/json",
                          "",
                          true);
        }
        else
        {
            system(string("\"/mnt/c/Program Files (x86)/Internet Download Manager/idman.exe\" /d \"" + VideoList.front().second + "\" /p \"D:\" /f \"" + VideoList.front().first + "\"").c_str());
            sleep(13);
        }
#ifdef TEST
        CheckFileName = VideoList.front().first;
#endif
        VideoList.pop();
#ifdef TEST
        break;
#endif
    }
#ifdef TEST
    MD5 MD5Encoder;
    cout << "Calculating MD5... " << flush;
    string MD5Value = MD5Encoder.encode(GetDataFromFileToString(CheckFileName));
    cout << "Success" << endl;
    string CorrectMD5Value = "3955a0da0d5890df8bb04cce8226d716";
    remove(CheckFileName.c_str());
    if (MD5Value != CorrectMD5Value)
    {
        OutputSummary("MD5 check failed. ");
        OutputSummary("Correct MD5 is " + CorrectMD5Value);
        OutputSummary("Current MD5 is " + MD5Value);
    }
    else
    {
        OutputSummary("Success");
    }
#endif
    CLN_CATCH
    return 0;
}
