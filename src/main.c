#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <whapi/whapi.h>

#include "argparse.h"

#ifdef WH_OS_WINDOWS
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static void sleep_ms(int milliseconds);

static bool handler(unsigned int response);

static const char seed_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

static const char *const usages[] = {"whcli [args]", NULL};

int main(int argc, const char *argv[]) {
    SearchParameters params = {.ratio_count = 1,
                               .ratios = (Ratio[]){{.width = 16, .height = 9}}};

    const char *type = NULL;
    const char *dir = NULL;
    const char *apikey = NULL;
    int set_seed;

    struct argparse_option options[] = {
        OPT_HELP(),

        OPT_STRING('k', "apikey", &apikey, "The apikey", NULL, 0, 0),

        OPT_GROUP("Query options:"),
        OPT_INTEGER('i', "id", &params.q.id, "Tag Id for exact id search", NULL,
                    0, 0),
        OPT_STRING(
            'q', "query", &params.q.tags,
            "Tag search (tag -> fuzzy search, +tag -> must include, -tag -> "
            "must not include)",
            NULL, 0, 0),
        OPT_STRING('u', "user_name", &params.q.user_name, "User uploads search",
                   NULL, 0, 0),
        OPT_STRING('t', "type", &type,
                   "Type of image (Should be png or jpg or jpeg)", NULL, 0, 0),
        OPT_STRING('l', "like", &params.q.like,
                   "Wallpapers like (give wallpaper id)", NULL, 0, 0),

        OPT_GROUP("Other options:"),
        OPT_STRING('d', "dir", &dir, "Directory to download", NULL, 0, 0),
        OPT_BIT(0, "sfw", &params.purity, "Safe For Work", NULL, PURITY_SFW, 0),
        OPT_BIT(0, "sketchy", &params.purity, "Sketchy", NULL, PURITY_SKETCHY,
                0),
        OPT_BIT(0, "nsfw", &params.purity, "Not Safe For Work", NULL,
                PURITY_NSFW, 0),
        OPT_BIT(0, "purity-all", &params.purity, "sfw | sketch | nsfw", NULL,
                PURITY_ALL, 0),
        OPT_BIT(0, "general", &params.categories, "General", NULL,
                CATEGORY_GENERAL, 0),
        OPT_BIT(0, "anime", &params.categories, "Anime", NULL, CATEGORY_ANIME,
                0),
        OPT_BIT(0, "people", &params.categories, "People", NULL,
                CATEGORY_PEOPLE, 0),
        OPT_BIT(0, "category-all", &params.categories,
                "general | anime | people", NULL, CATEGORY_ALL, 0),
        OPT_INTEGER('p', "page", &params.page, "Page number", NULL, 0, 0),
        OPT_BOOLEAN('s', "seed", &set_seed, "Use seed", NULL, 0, 0),

        OPT_END()};

    struct argparse argparse;

    argparse_init(&argparse, options, usages, 0);

    argparse_describe(&argparse, "\nDownload wallpapers from wallhaven", NULL);

    argc = argparse_parse(&argparse, argc, argv);
    if (argc) {
        printf("ERROR: Extra arguments\n");
        return -1;
    }

    if (type) {
        switch (type[0]) {
            case 'p': {
                if (type[1] != 'n' || type[2] != 'g' || type[3] != 0) goto err;
                params.q.type = IMAGE_TYPE_PNG;
            } break;
            case 'j': {
                if (type[1] != 'p') goto err;
                if (type[2] == 'e') {
                    if (type[3] != 'g' || type[4] != 0) goto err;
                } else if (type[2] != 'g' || type[3] != 0) {
                    goto err;
                }
                params.q.type = IMAGE_TYPE_JPG;
            } break;
            default:
            err:
                printf("Invalid argument for type\n");
                return -1;
        }
    }

    if (dir) {
        int i;
        for (i = 0; dir[i]; ++i);
        if (dir[i - 1] == '/' || dir[i - 1] == '\\') {
            printf("Remove the last '/' or '\\' in the path");
        }
    }

    if (apikey) whapi_set_apikey(apikey);

    if (!whapi_initialize(true)) {
        printf("Failed to initialize whapi\n");
        whapi_shutdown();
        return -1;
    }

    if (set_seed) {
        srand(time(NULL));
        for (int i = 0; i < 6; ++i) params.seed[i] = seed_chars[rand() % 62];
    }

    whapi_set_response_code_handler(handler);

    SearchResult result;
    if (!whapi_search(params, &result)) {
        printf("Search failed\n");
        whapi_shutdown();
        return -1;
    }

    for (int i = 0; i < result.wallpaper_count; ++i) {
        printf("Downloading wallpaper: %s...\n", result.wallpapers[i].id);
        if (!whapi_download_wallpaper(&result.wallpapers[i], dir ? dir : ".")) {
            printf("Failed to download wallpaper: %s\n",
                   result.wallpapers[i].id);
        }
    }

    whapi_destroy_search_result(&result);

    whapi_shutdown();
}

static void sleep_ms(int milliseconds) {
#ifdef WH_OS_WINDOWS
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

static bool handler(unsigned int response) {
#ifdef WH_DEBUG
    printf("Response code: %u\n", response);
#endif
    switch (response) {
        case 200:
#ifdef WH_DEBUG
            printf("Success!\n");
#endif
            break;
        case 429:
            printf("Sleeping for 15 seconds\n");
            sleep_ms(1000 * 15);
            return true;
        default:
            break;
    }

    return false;
}
