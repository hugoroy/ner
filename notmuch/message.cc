/* ner: notmuch/message.cc
 *
 * Copyright (c) 2012 Michael Forney
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

#include "message.hh"
#include "database.hh"

namespace Notmuch
{
    Message::Message(notmuch_message_t * message, Parts parts)
    {
        if (parts & MetadataPart)
        {
            id = notmuch_message_get_message_id(message);
            subject = notmuch_message_get_header(message, "subject");
            date = std::chrono::system_clock::from_time_t(
                notmuch_message_get_date(message));

            headers.insert(std::make_pair("from",
                notmuch_message_get_header(message, "from") ?: "(null)"));

            notmuch_tags_t * notmuch_tags;
            for (notmuch_tags = notmuch_message_get_tags(message);
                notmuch_tags_valid(notmuch_tags);
                notmuch_tags_move_to_next(notmuch_tags))
            {
                tags.insert(notmuch_tags_get(notmuch_tags));
            }
        }

        if (parts & FilenamePart)
            filename = notmuch_message_get_filename(message);
    }

    bool Message::perform_tag_operations(const TagOperations & ops)
    {
        Database database(Database::Mode::ReadWrite);

        notmuch_message_t * message;
        notmuch_status_t status;

        status = notmuch_database_find_message(database.get(),
            id.c_str(), &message);
        if (status != NOTMUCH_STATUS_SUCCESS)
            return false;

        if ((status = notmuch_message_freeze(message)) != NOTMUCH_STATUS_SUCCESS)
            return false;

        for (auto & op : ops)
        {
            switch (op.type)
            {
                case TagOperation::Add:
                    notmuch_message_add_tag(message, op.tag.c_str());
                    break;
                case TagOperation::Remove:
                    notmuch_message_remove_tag(message, op.tag.c_str());
                    break;
                case TagOperation::Clear:
                    notmuch_message_remove_all_tags(message);
                    break;
            }
        }

        if ((status = notmuch_message_thaw(message)) != NOTMUCH_STATUS_SUCCESS)
            return false;

        database.close();

        /* Now that the tag operations have been applied, we can apply these
         * operations to our message structure. */
        for (auto & op : ops)
        {
            switch (op.type)
            {
                case TagOperation::Add:
                    tags.insert(op.tag);
                    break;
                case TagOperation::Remove:
                    tags.erase(tags.find(op.tag));
                    break;
                case TagOperation::Clear:
                    tags.clear();
                    break;
            }
        }

        return true;
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

