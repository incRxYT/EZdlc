#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

// ──────────────────────────────────────────────
//  Known DLC map file prefixes (zone / snd / video)
//  Extend this list as needed.
// ──────────────────────────────────────────────
static const std::unordered_set<std::string> KNOWN_DLC_PREFIXES = {
    // DLC 1 – Awakening
    "mp_algeria", "mp_rise", "mp_skyjacked", "mp_splash",
    "zm_shadows",
    // DLC 2 – Eclipse
    "mp_chinatown", "mp_discord", "mp_redwood", "mp_spire",
    "zm_zetsubou",
    // DLC 3 – Descent
    "mp_banzai", "mp_cryogen", "mp_metro", "mp_veiled",
    "zm_gorod",
    // DLC 4 – Salvation
    "mp_havoc", "mp_infection", "mp_medieval", "mp_ruins",
    "zm_genesis",
    // Zombies Chronicles
    "zm_nacht", "zm_verruckt", "zm_shi_no_numa", "zm_der_riese",
    "zm_kino", "zm_ascension", "zm_call_of_the_dead", "zm_shangri_la",
    "zm_moon", "zm_buried", "zm_mob_of_the_dead", "zm_origins"
};

enum class InjectMode { COPY, SYMLINK };

struct Config {
    fs::path srcZone;
    fs::path srcSndEn;
    fs::path srcVideo;
    fs::path srcAll;
    fs::path dstZone;
    fs::path dstSndEn;
    fs::path dstVideo;
    fs::path dstAll;
    InjectMode mode = InjectMode::COPY;
    bool dryRun = false;
};

struct FileOp {
    fs::path src;
    fs::path dst;
    bool isSymlink;
};

// ──────────────────────────────────────────────
//  Helpers
// ──────────────────────────────────────────────

bool matchesDlcPrefix(const std::string& stem) {
    std::string lower = stem;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& prefix : KNOWN_DLC_PREFIXES) {
        if (lower.rfind(prefix, 0) == 0)
            return true;
    }
    return false;
}

std::string modeLabel(InjectMode m) {
    return m == InjectMode::COPY ? "copy" : "symlink";
}

void printSeparator() {
    std::cout << std::string(60, '-') << "\n";
}

fs::path promptPath(const std::string& label) {
    std::string input;
    std::cout << "  " << label << ": ";
    std::getline(std::cin, input);
    // Trim surrounding quotes that Windows users often paste
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"')
        input = input.substr(1, input.size() - 2);
    return fs::path(input);
}

bool confirmPrompt(const std::string& msg) {
    std::string ans;
    std::cout << msg << " [y/N] ";
    std::getline(std::cin, ans);
    return !ans.empty() && (ans[0] == 'y' || ans[0] == 'Y');
}

// ──────────────────────────────────────────────
//  Scan a source folder and collect DLC files
// ──────────────────────────────────────────────
std::vector<FileOp> scanFolder(
    const fs::path& srcDir,
    const fs::path& dstDir,
    InjectMode mode)
{
    std::vector<FileOp> ops;
    if (!fs::exists(srcDir)) return ops;

    for (const auto& entry : fs::recursive_directory_iterator(
            srcDir, fs::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) continue;

        const fs::path& srcPath = entry.path();
        std::string stem = srcPath.stem().string();

        if (!matchesDlcPrefix(stem)) continue;

        // Mirror relative path into destination
        fs::path rel = fs::relative(srcPath, srcDir);
        fs::path dstPath = dstDir / rel;

        ops.push_back({ srcPath, dstPath, mode == InjectMode::SYMLINK });
    }
    return ops;
}

// ──────────────────────────────────────────────
//  Apply a single file operation
// ──────────────────────────────────────────────
bool applyOp(const FileOp& op) {
    std::error_code ec;

    // Ensure destination parent exists
    fs::create_directories(op.dst.parent_path(), ec);
    if (ec) {
        std::cerr << "  [ERROR] Could not create dir: "
                  << op.dst.parent_path() << " – " << ec.message() << "\n";
        return false;
    }

    // Remove existing file/link at destination
    if (fs::exists(op.dst) || fs::is_symlink(op.dst))
        fs::remove(op.dst, ec);

    if (op.isSymlink) {
        fs::create_symlink(op.src, op.dst, ec);
    } else {
        fs::copy_file(op.src, op.dst,
                      fs::copy_options::overwrite_existing, ec);
    }

    if (ec) {
        std::cerr << "  [ERROR] " << (op.isSymlink ? "symlink" : "copy")
                  << " failed: " << op.dst << " – " << ec.message() << "\n";
        return false;
    }
    return true;
}

// ──────────────────────────────────────────────
//  Entry point
// ──────────────────────────────────────────────
int main() {
    std::cout << "\n";
    std::cout << "  ██████╗  ██████╗ ██████╗     ██████╗ ██╗      ██████╗\n";
    std::cout << "  ██╔══██╗██╔═══██╗╚════██╗    ██╔══██╗██║     ██╔════╝\n";
    std::cout << "  ██████╔╝██║   ██║ █████╔╝    ██║  ██║██║     ██║\n";
    std::cout << "  ██╔══██╗██║   ██║ ╚═══██╗    ██║  ██║██║     ██║\n";
    std::cout << "  ██████╔╝╚██████╔╝██████╔╝    ██████╔╝███████╗╚██████╗\n";
    std::cout << "  ╚═════╝  ╚═════╝ ╚═════╝     ╚═════╝ ╚══════╝ ╚═════╝\n";
    std::cout << "\n  BoIII DLC Injector  –  uses YOUR files, no piracy\n\n";
    printSeparator();

    Config cfg;

    // ── Source paths (vanilla BO3 install) ──
    std::cout << "\n[1/2] Your vanilla BO3 install paths\n";
    cfg.srcZone  = promptPath("zone folder   (e.g. C:\\BO3\\zone)");
    cfg.srcSndEn = promptPath("snd/en folder (e.g. C:\\BO3\\sound\\raw\\en)");
    cfg.srcVideo = promptPath("video folder  (e.g. C:\\BO3\\video)");
    cfg.srcAll   = promptPath("all folder    (e.g. C:\\BO3\\all)");

    for (const auto& p : { cfg.srcZone, cfg.srcSndEn, cfg.srcVideo, cfg.srcAll }) {
        if (!fs::exists(p)) {
            std::cerr << "\n  [ERROR] Path does not exist: " << p << "\n";
            return 1;
        }
    }

    // ── Destination paths (BoIII install) ──
    std::cout << "\n[2/2] Your BoIII install paths\n";
    cfg.dstZone  = promptPath("zone folder   (e.g. C:\\BoIII\\zone)");
    cfg.dstSndEn = promptPath("snd/en folder (e.g. C:\\BoIII\\sound\\raw\\en)");
    cfg.dstVideo = promptPath("video folder  (e.g. C:\\BoIII\\video)");
    cfg.dstAll   = promptPath("all folder    (e.g. C:\\BoIII\\all)");

    // ── Mode selection ──
    std::cout << "\nInject mode:\n"
              << "  1) Copy files  (safe, uses extra disk space)\n"
              << "  2) Symlink     (saves space, requires admin on Windows)\n"
              << "Choice [1/2]: ";
    std::string modeInput;
    std::getline(std::cin, modeInput);
    cfg.mode = (modeInput == "2") ? InjectMode::SYMLINK : InjectMode::COPY;

    // ── Dry run? ──
    cfg.dryRun = confirmPrompt("\nDry run first (preview without making changes)?");

    // ──────────────────────────────────────────
    //  Scan all three folder pairs
    // ──────────────────────────────────────────
    printSeparator();
    std::cout << "\nScanning for DLC files...\n\n";

    auto zoneOps  = scanFolder(cfg.srcZone,  cfg.dstZone,  cfg.mode);
    auto sndOps   = scanFolder(cfg.srcSndEn, cfg.dstSndEn, cfg.mode);
    auto videoOps = scanFolder(cfg.srcVideo, cfg.dstVideo, cfg.mode);
    auto allOps   = scanFolder(cfg.srcAll,   cfg.dstAll,   cfg.mode);

    std::vector<FileOp> allOpsList;
    allOpsList.insert(allOpsList.end(), zoneOps.begin(),  zoneOps.end());
    allOpsList.insert(allOpsList.end(), sndOps.begin(),   sndOps.end());
    allOpsList.insert(allOpsList.end(), videoOps.begin(), videoOps.end());
    allOpsList.insert(allOpsList.end(), allOps.begin(),   allOps.end());

    if (allOpsList.empty()) {
        std::cout << "  No DLC files found in the source folders.\n"
                  << "  Make sure your BO3 install has DLC content downloaded via Steam.\n\n";
        return 0;
    }

    // ── Print preview ──
    std::cout << "  Found " << allOpsList.size() << " DLC file(s) to "
              << modeLabel(cfg.mode) << ":\n\n";

    for (const auto& op : allOpsList) {
        std::cout << "  [" << (op.isSymlink ? "LINK" : "COPY") << "] "
                  << op.src.filename().string() << "\n"
                  << "       → " << op.dst << "\n";
    }
    std::cout << "\n";
    printSeparator();

    if (cfg.dryRun) {
        std::cout << "\n  Dry run complete. No files were modified.\n\n";
        if (!confirmPrompt("Apply these changes now?"))
            return 0;
    }

    // ──────────────────────────────────────────
    //  Apply operations
    // ──────────────────────────────────────────
    std::cout << "\nApplying...\n\n";

    int ok = 0, fail = 0;
    for (const auto& op : allOpsList) {
        if (applyOp(op)) {
            std::cout << "  [OK]   " << op.src.filename().string() << "\n";
            ++ok;
        } else {
            ++fail;
        }
    }

    printSeparator();
    std::cout << "\n  Done.  " << ok << " succeeded";
    if (fail > 0) std::cout << ", " << fail << " failed";
    std::cout << ".\n\n";

    if (fail > 0)
        std::cout << "  Tip: if symlinks fail on Windows, run as Administrator.\n\n";

    return fail > 0 ? 1 : 0;
}
