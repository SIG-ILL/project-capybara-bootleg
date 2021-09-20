#pragma once

namespace pcb {
	template <class T>
	class BoundsPair final {
	public:
		T lower;
		T upper;
	};
}