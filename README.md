# secure-notes-exporter

Exports the "Secure Notes" from your OS X keychain into a CSV file that can be
imported into [LastPass](https://lastpass.com/).

## Usage

1. Clone the repo.
2. Open the Xcode project file and Build (alternatively, just run `make` from the terminal).
3. Install built binary somewhere in your `PATH`.
4. Run the binary; the decrypted contents of the notes will be printed to standard out:

```bash
secure-notes-exporter > ~/notes.csv
```

You will have to click the "Allow" (or "Always Allow") button in the dialog that
pops up for each "Secure Note" in your keychain; it's a little painful, but
still better than manually copying and pasting each individual item into
LastPass.

**NOTE:** The generated CSV contains the decrypted plain-text of your "Secure
Notes", so make sure you keep it somewhere safe until you import it into
LastPass, and securely delete it immediately afterwards.

## Importing the generated CSV into LastPass

For up-to-date instructions, consult
[the LastPass website](https://helpdesk.lastpass.com/getting-started/importing-from-other-password-managers/#Importing+from+a+Generic+CSV+File).

At the time of writing, the procedure is:

1. From the LastPass browser extension menu, choose "Tools" > "Import From" >
   "Other"
2. On the page that appears, choose "Generic CSV File" from the "Source" menu.
3. Click "Import"
4. Choose your exported CSV file
5. Follow the on-screen prompts

## Background

For many years, I used the OS X keychain to store all of my passwords, and
additionally information that was sensitive but wasn't a password. I did this
because I wanted to avoid vendor lock-in with a small company such as
[AgileBits](https://agilebits.com/) (makers of
[1Password](https://agilebits.com/onepassword/mac)), which at the time was
completely unproven (the [first alphas](https://agilebits.com/onepassword/mac/release_notes)
of 1Password came out in 2006).

Of course, I knew that I still wasn't free from vendor lock-in, as I was
effectively locking myself into Apple's proprietary keychain format and
implementation. Nevertheless, I felt better about this because I knew that I was
likely to be an Apple user for a very long time. They'd also done much to earn
my trust in terms of their relatively good security record.

Unfortunately, each upgrade to a new machine brought with it some pain as I
either had to blow away the login keychain on the new machine (by replacing it
with the old keychain), or I had to manually merge the keychains.

This latter process was particularly tedious. Even if you set a blank password
on your keychain to expedite things, you still had to click on a permission
dialog for every single item you wished to import. Ouch.

Finally, after much deliberation I decided to give in and move my sensitive
information to LastPass. I was sick of these keychain migrations and decided to
do one last migration to end them all. Additionally, as I become more and more
mobile-dependent, I wanted to be able to access the exact same passwords on all
of my devices. (Apple had recently come out with iCloud Keychain, but I tried it
and found it to contain a weak, insufficient subset of LastPass's features.)

The spectre of vendor lock-in looms large with LastPass, of course, but I feel
they've done well to establish a track record. I can export my vault to XML too,
which means I can back it up in a non-proprietary format.

LastPass is truly ugly, hideously ugly, but I think it's the best choice out
there for features, cross-platform support and security. Additionally, it is
fairly priced.

Migrating my passwords into LastPass wasn't too bad using tools like [this
script](https://gist.github.com/rwest/1583781). But there was nothing to get my
200+ "Secure Notes" migrated short of copying and pasting them. I looked at
using the `security` command-line tool that comes with OS X, but it doesn't
export the information in a very useful format.

So, with a few hours of hacking I produced `secure-notes-exporter`. The code is
ugly, and I'm sure my lack of knowledge about Core Foundation idioms shows, but
the code works.
