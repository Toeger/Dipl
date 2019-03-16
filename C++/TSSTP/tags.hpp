#pragma once

namespace TST {
	enum Ownership : bool { Non_owner, Owner };
	enum Convertibility : char { None, Checkable, Waitable, Unique };
	enum Share : bool { Non_shared, Shared };
} // namespace TST