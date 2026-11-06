#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <openssl/sha.h>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "../../core/menu.hpp"
#include "../../core/visual/banner.hpp"

static std::string get_input_line() {
  std::string s;
  std::getline(std::cin, s);
  return s;
}

static std::string edit_in_editor(const std::string &initial = "") {
  namespace fs = std::filesystem;
  fs::create_directories("data");
  fs::path tmp =
      fs::path("data") / ("tmp_note_" + std::to_string(getpid()) + ".txt");
  {
    std::ofstream out(tmp);
    out << initial;
  }

  const char *ed = std::getenv("EDITOR");
  if (!ed || !*ed)
    ed = "nano";
  std::string cmd = std::string(ed) + " \"" + tmp.string() + "\"";
  std::system(cmd.c_str());

  std::ifstream in(tmp);
  std::stringstream ss;
  ss << in.rdbuf();
  in.close();
  fs::remove(tmp);
  return ss.str();
}

static std::string notes_dir() { return "data/notes"; }

static void ensure_dirs() {
  namespace fs = std::filesystem;
  fs::create_directories(notes_dir());
  chmod("data", S_IRWXU);
}

static std::string make_id() {
  return std::to_string(std::time(nullptr)) + "_" + std::to_string(getpid());
}

static std::string note_path(const std::string &id) {
  return notes_dir() + "/" + id + ".txt";
}

static void save_note(const std::string &id, const std::string &title,
                      const std::string &body) {
  ensure_dirs();
  std::ofstream f(note_path(id), std::ofstream::trunc);
  if (!f.is_open()) {
    std::cerr << "ERROR: cannot open " << note_path(id) << std::endl;
    return;
  }
  f << title << "\n";
  f << body;
  f.close();
  chmod(note_path(id).c_str(), S_IRUSR | S_IWUSR);
}

static std::pair<std::string, std::string> load_note(const std::string &id) {
  std::ifstream f(note_path(id));
  std::string title, body, line;
  if (!std::getline(f, title))
    return {"", ""};
  std::stringstream ss;
  while (std::getline(f, line))
    ss << line << "\n";
  return {title, ss.str()};
}

static std::string prompt_line(const std::string &label) {
  std::cout << label;
  std::cout.flush();
  return get_input_line();
}

static void new_note() {
  std::string title = prompt_line("\n  Title: ");
  std::string body = edit_in_editor("");
  save_note(make_id(), title, body);
}

static void list_notes_and_pick() {
  namespace fs = std::filesystem;
  ensure_dirs();

  std::vector<std::string> ids;
  for (auto &e : fs::directory_iterator(notes_dir())) {
    if (e.path().extension() == ".txt") {
      ids.push_back(e.path().stem().string());
    }
  }
  std::sort(ids.begin(), ids.end());

  if (ids.empty()) {
    return;
  }

  std::cout << "\n";
  for (size_t i = 0; i < ids.size(); ++i) {
    auto [title, _] = load_note(ids[i]);
    std::cout << "  [" << (i + 1) << "] " << title << "\n";
  }

  std::string pick = prompt_line("\n  > ");
  if (pick == "0" || pick.empty())
    return;

  int idx = 0;
  try {
    idx = std::stoi(pick);
  } catch (...) {
    return;
  }
  if (idx < 1 || idx > (int)ids.size()) {
    return;
  }

  std::string id = ids[idx - 1];
  auto [title, body] = load_note(id);
  std::string edited = edit_in_editor(body);
  save_note(id, title, edited);
}

static void delete_note() {
  namespace fs = std::filesystem;
  ensure_dirs();

  std::vector<std::string> ids;
  for (auto &e : fs::directory_iterator(notes_dir())) {
    if (e.path().extension() == ".txt")
      ids.push_back(e.path().stem().string());
  }
  std::sort(ids.begin(), ids.end());

  if (ids.empty()) {
    return;
  }
  std::cout << "\n";
  for (size_t i = 0; i < ids.size(); ++i) {
    auto [title, _] = load_note(ids[i]);
    std::cout << "  [" << (i + 1) << "] " << title << "\n";
  }
  std::string pick = prompt_line("\n  > ");
  if (pick == "0" || pick.empty())
    return;
  int idx = 0;
  try {
    idx = std::stoi(pick);
  } catch (...) {
    return;
  }
  if (idx < 1 || idx > (int)ids.size())
    return;
  fs::remove(note_path(ids[idx - 1]));
}

static void show_note() {
  namespace fs = std::filesystem;
  ensure_dirs();

  std::vector<std::string> ids;
  for (auto &e : fs::directory_iterator(notes_dir())) {
    if (e.path().extension() == ".txt")
      ids.push_back(e.path().stem().string());
  }
  std::sort(ids.begin(), ids.end());

  if (ids.empty()) {
    return;
  }
  std::cout << "\n";
  for (size_t i = 0; i < ids.size(); ++i) {
    auto [title, _] = load_note(ids[i]);
    std::cout << "  [" << (i + 1) << "] " << title << "\n";
  }
  std::string pick = prompt_line("\n  > ");
  if (pick == "0" || pick.empty())
    return;
  int idx = 0;
  try {
    idx = std::stoi(pick);
  } catch (...) {
    return;
  }
  if (idx < 1 || idx > (int)ids.size())
    return;

  auto [title, body] = load_note(ids[idx - 1]);
  std::cout << "\n" << title << "\n" << body;
  if (!body.empty() && body.back() != '\n')
    std::cout << "\n";
  std::cout << "\n";
  prompt_line("  > ");
}

void localspace() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  while (true) {
    clear_console();
    main_banner();

    std::cout << "\n"
                 "  [ 1 ] NEW NOTE\n"
                 "  [ 2 ] LIST / OPEN\n"
                 "  [ 3 ] DELETE\n"
                 "  [ 4 ] SHOW\n"
                 "  [ 0 ] BACK\n\n"
                 "  > ";
    std::cout.flush();

    std::string choice = get_input_line();
    if (choice == "0" || choice.empty())
      return;
    if (choice == "1") {
      new_note();
    } else if (choice == "2") {
      list_notes_and_pick();
    } else if (choice == "3") {
      delete_note();
    } else if (choice == "4") {
      show_note();
    }
  }
}
