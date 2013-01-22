/* ner: src/ner_config.hh
 *
 * Copyright (c) 2010 Michael Forney
 *
 * This file is a part of ner.
 *
 * ner is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3, as published by the Free
 * Software Foundation.
 *
 * ner is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ner.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NER_NER_CONFIG_H
#define NER_NER_CONFIG_H 1

#include <map>
#include <string>

#include "search_list_view.hh"
#include "notmuch/query.hh"

class NerConfig
{
    public:
        static const NerConfig & instance();

        void load();

        std::map<std::string, std::string> commands;
        std::vector<Search> searches;
        Notmuch::SortMode sort_mode;
        bool refresh_view;
        bool add_signature_dashes;
        ColorMap color_map;

    private:
        NerConfig();

        static const NerConfig * _instance;

    friend int main(int, char **);
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

