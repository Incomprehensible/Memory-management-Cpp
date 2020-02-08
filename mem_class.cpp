#include <iostream>
#include <stdlib.h>
#include <memory>
#include <map>

using namespace std;

#define MAX_MEM_SIZE 200

class SmallAllocator {
private:
	char	arr[MAX_MEM_SIZE];
	map<void *, unsigned int> entries;
	unsigned int	capacity = MAX_MEM_SIZE - 1;

public:
	SmallAllocator() {
		memset(&arr, 0, MAX_MEM_SIZE - 1);
	}
	~SmallAllocator()
	{
		entries.clear();
		//memset(&arr, 0, MAX_MEM_SIZE - 1);
	}
    void *Alloc(unsigned int Size);
    void *ReAlloc(void *Pointer, unsigned int Size);
    void Free(void *Pointer);
	void	Show()
	{
		for (auto i: entries)
		{
			cout << "address: " << i.first << " size: " << i.second << endl;
		}
	}
};

void*	SmallAllocator::Alloc(unsigned int Size)
{
	unsigned int last_size(0);
	void		*ptr(nullptr);
	unsigned int	max_size = MAX_MEM_SIZE - 1;

	if (!Size || capacity < Size)
		return (nullptr);
	if (entries.empty())
	{
		entries.insert(make_pair(&arr[0], Size));
		return (&arr[0]);
	}
	for (auto i = entries.begin(); i != entries.end(); ++i)
	{
		ptr = i->first ? i->first : ptr;
		last_size = i->second? i->second : last_size;
		max_size -= last_size;
	}
	if (ptr && last_size && max_size >= Size)
	{
		ptr = (char *)ptr + last_size;
		entries.insert(std::make_pair(ptr, Size));
	}
	else
		return (nullptr);
	capacity -= Size;
	return (ptr);
}

void	SmallAllocator::Free(void *Pointer)
{
	void	*ptr_remv;
	unsigned int	offset;

	if (entries.empty())
		return;
	auto entry = entries.find(Pointer);
	if (entry->first && entry->second)
	{
		ptr_remv = entry->first;
		memset(ptr_remv, 0, entry->second);
		offset = entry->second;
		auto next = entries.find((char *)ptr_remv + offset);
		entries.erase(entry);
		capacity += offset;
		Pointer = nullptr;
		// while (next->first && next->second)
		// {
		// 	cout << "next found" << endl;
		// 	unsigned char tmp[next->second];
		// 	memcpy(tmp, next->first, next->second);
		// 	memset(next->first, 0, next->second);
		// 	auto next_next = (char *)next->first + next->second;
		// 	entries.insert(make_pair((char *)next->first - offset, next->second));
		// 	memcpy((char *)next->first - offset, tmp, next->second);
		// 	entries.erase(next);
		// 	next = entries.find(next_next);
		// }
	}
}

void*	SmallAllocator::ReAlloc(void *Pointer, unsigned int Size)
{
	unsigned int	last_size(0);
	void			*ptr(nullptr);
	void			*start(nullptr);
	unsigned int	rem_size(MAX_MEM_SIZE - 1);

	if (!Size || !Pointer || capacity < Size)
		return Pointer;
	if (entries.empty())
		ptr = &arr[0];
	cout << "old_ptr: " << Pointer << endl;
	auto old_size = entries.find((char *)Pointer)->second;
	cout << "old_size: " << old_size << endl;
	cout << "new_size: " << Size << endl;
	start = &arr[0];
	for (auto i = entries.begin(); i != entries.end(); ++i)
	{
		if (start)
		{
			if ((ptrdiff_t)((char *)i->first - (char *)start) >= Size)
			{
				ptr = start;
				break;
			}
		}
		start = i->first ? i->first : ptr;
		cout << "start: " << start << endl;
		last_size = i->second? i->second : 0;
		cout << "last_size: " << last_size << endl;
		start = (char *)start + last_size;
		cout << "start after: " << start << endl;
		rem_size -= last_size;
		cout << "rem_size: " << rem_size << endl;
	}
	if (!ptr && start)
	{
		if (rem_size >= Size)
		{
			ptr = start;
			memcpy(ptr, "test", 4);
			cout << "new_ptr: " << ptr << endl;
		}
	}
	if (ptr)
	{
		unsigned char tmp[old_size];
		memcpy(tmp, Pointer, old_size);
		if (old_size > Size)
			memcpy(ptr, tmp, Size);
		else
			memcpy(ptr, tmp, old_size);
		memset(Pointer, 0, old_size);
		// 	auto next_next = (char *)next->first + next->second;
		// 	entries.insert(make_pair((char *)next->first - offset, next->second));
		// 	memcpy((char *)next->first - offset, tmp, next->second);
		// 	entries.erase(next);
		entries.erase(Pointer);
		entries.insert(std::make_pair(ptr, Size));
		Pointer = nullptr;
	}
	else
		return (nullptr);
	capacity += old_size - Size;
	return (ptr);
}

int		main()
{
	SmallAllocator All1;
	int * A1 = (int *) All1.Alloc(sizeof(int));
	*A1 = 16;
	cout << "A1: " << *A1 << endl;
	int * A2 = (int *) All1.Alloc(sizeof(int));
	*A2 = 32;
	cout << "A2: " << *A2 << endl;
	char * A3 = (char *) All1.Alloc(sizeof(char) * 10);
	A3[9] = '\0';
	memcpy(A3, "kekkek", 7);
	cout << "A3: " << A3 << endl;
	cout << endl << endl;
	A3 = (char *)All1.ReAlloc(A3, sizeof(char) * 100);
	cout << endl;
	A1 = (int *)All1.ReAlloc(A1, sizeof(int) * 4);
	cout << *A1 << endl;
	cout << A3 << endl;
	All1.Show();
	int *no = (int *) All1.Alloc(sizeof(int));
	if (!no)
		cout << "I got nullptr" << endl;
	else
		*no = 3;
	cout << *no << endl;
	All1.Free(A1);
	cout << "removed A1" << endl;
	All1.Show();
	All1.Free(A2);
	cout << "removed A2" << endl;
	All1.Show();
	cout << A3 << endl;
	All1.Free(A3);
	cout << "removed A3" << endl;
	All1.Show();
	cout << A3 << endl;


	int * A4 = (int *) All1.Alloc(sizeof(int));
	*A4 = 25;
	cout << "A4: " << *A4 << endl;
	int * A5 = (int *) All1.Alloc(sizeof(int) * 2);
	*A5 = 64;
	cout << "A5: " << *A5 << endl;
	char * A6 = (char *) All1.Alloc(sizeof(char) * 10);
	A6[9] = '\0';
	memcpy(A6, "wowwow", 7);
	cout << "A6: " << A6 << endl;
	int * nono = (int *) All1.Alloc(sizeof(int) * 3);
	if (!nono)
		cout << "I got nullptr" << endl;
	else
		*nono = 800;
	cout << *nono << endl << endl;
	All1.Show();
	All1.Free(no);
	A5 = (int *)All1.ReAlloc(A5, sizeof(int) * 8);
	cout << endl << endl << endl;
	A6 = (char *)All1.ReAlloc(A6, 100);
	if (!A5 || !A6)
		cout << "I got nullptr" << endl;
	A6 = (char *)All1.ReAlloc(A6, 16);
	All1.Show();
	if (!A5 || !A6)
		cout << "I got nullptr" << endl;
	All1.Free(nono);
	A6 = (char *)All1.ReAlloc(A6, 60);
	All1.Free(A5);
	cout << *A4 << endl;
	cout << endl << endl;
	All1.Show();
	return 0;
}