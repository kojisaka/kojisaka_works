package com.example.demo.repository;

import java.util.List;

import jakarta.persistence.LockModeType;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Lock;

import com.example.demo.entity.DepStatusEntityEx;

public interface DepStatusRepositoryEx
	extends JpaRepository<DepStatusEntityEx, Integer> {

	@Lock(LockModeType.PESSIMISTIC_WRITE)
	public List<DepStatusEntityEx>
		findByNameAndType(String name, String type);

	public void deleteByNameAndType(String name, String type);
}
