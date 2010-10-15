/* a PEG.js[1] CSV parser modeled after an article[2] by Julian M Bucknall 
 *
 *    [1] http://pegjs.majda.cz/
 *    [2] http://www.boyet.com/articles/csvparser.html
 */

csv_file
    = csv_record *

csv_record
    = values:csv_string_list new_line { return values; }

csv_string_list
    = first:raw_string more:(';' raw_string) * {
        var result = [first];
        for (var i = 0; i < more.length; i++) {
            result.push(more[i][1]);
        }
        return result;
    }

raw_string
    = [ \t]* field:(raw_field [ \t]*) ? { return field ? field[0] : undefined; }

raw_field
    = simple_field
    / quoted_field

simple_field
    = chars:[^\n\t ;"] + { return chars.join(''); }

quoted_field
    = '"' value:escaped_field '"' { return value; }

escaped_field
    = first:sub_field more:('""' sub_field) * {
        var result = first;
        for (var i = 0; i < more.length; i++) {
            result += '"' + more[i][1];
        }
        return result;
    }

sub_field
    = chars:[^"] * { return chars.join(''); }

new_line
    = '\r\n'
    / '\n'

